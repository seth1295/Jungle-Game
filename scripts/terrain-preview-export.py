#!/usr/bin/env python3
"""Export real PR5 Batch 004 x6 island preview images from deterministic terrain data.

This tool is deliberately not AI/image-generation. It mirrors the landed
`FJungleVolcanicIslandTerrainModel` terrain math and writes labelled PNG evidence
files from sampled height/mask data.

Default output is tracked terrain evidence under:
`Images/TerrainPreview/`

Default resolution is 1024 px so the primary island preview is readable without
needing UE rendering or full 8129 x 8129 export at 12 m/sample.
"""

from __future__ import annotations

import argparse
import json
import math
import struct
import time
import zlib
from pathlib import Path

WORLD_SIZE_M = 97536.0
HALF_EXTENT_M = WORLD_SIZE_M * 0.5
SEA_LEVEL_M = 0.0
MEAN_ISLAND_RADIUS_M = 42000.0
MAX_ISLAND_RADIUS_M = 44100.0
TARGET_PEAK_HEIGHT_M = 3800.0
PRIMARY_CATCHMENT_COUNT = 23
VERSION = "PR5_BATCH004_X6_ANTI_RADIAL_TERRAIN_PREVIEW"


def clamp(value: float, low: float, high: float) -> float:
    return max(low, min(high, value))


def lerp(a: float, b: float, t: float) -> float:
    return a + (b - a) * t


def smooth_step(edge0: float, edge1: float, value: float) -> float:
    if abs(edge0 - edge1) < 1.0e-6:
        return 1.0 if value >= edge1 else 0.0
    t = clamp((value - edge0) / (edge1 - edge0), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def ring_mask(distance_m: float, center_m: float, half_width_m: float) -> float:
    return 1.0 - smooth_step(half_width_m * 0.35, half_width_m, abs(distance_m - center_m))


def organic_island_radius_m(theta: float) -> float:
    broad_lobes = (
        1560.0 * math.sin(3.0 * theta + 0.70)
        + 1020.0 * math.sin(5.0 * theta - 1.10)
        + 450.0 * math.sin(9.0 * theta + 2.30)
        + 260.0 * math.sin(13.0 * theta - 0.80)
    )
    bay_headland_warp = 570.0 * math.sin(2.0 * theta - 0.35) * math.sin(7.0 * theta + 1.60)
    return clamp(MEAN_ISLAND_RADIUS_M + broad_lobes + bay_headland_warp, 37200.0, MAX_ISLAND_RADIUS_M)


def sample_terrain_m(world_x_m: float, world_y_m: float) -> dict[str, float | int]:
    radius_m = math.hypot(world_x_m, world_y_m)
    theta = math.atan2(world_y_m, world_x_m)
    island_radius_m = organic_island_radius_m(theta)
    signed_shoreline_distance_m = island_radius_m - radius_m
    land_mask = smooth_step(-10.0, 60.0, signed_shoreline_distance_m)

    seaward_distance_m = max(0.0, -signed_shoreline_distance_m)
    landward_distance_m = max(0.0, signed_shoreline_distance_m)

    outer_ocean_depth_m = lerp(-12.0, -145.0, smooth_step(250.0, 1000.0, seaward_distance_m))
    coastal_shelf_height_m = lerp(-2.0, -26.0, smooth_step(0.0, 700.0, seaward_distance_m))
    ocean_height_m = coastal_shelf_height_m if seaward_distance_m < 700.0 else outer_ocean_depth_m

    if landward_distance_m <= 90.0:
        coastal_land_height_m = lerp(0.0, 3.5, smooth_step(0.0, 90.0, landward_distance_m))
    elif landward_distance_m <= 240.0:
        coastal_land_height_m = lerp(3.5, 9.0, smooth_step(90.0, 240.0, landward_distance_m))
    elif landward_distance_m <= 1100.0:
        coastal_land_height_m = lerp(9.0, 62.0, smooth_step(240.0, 1100.0, landward_distance_m))
    else:
        coastal_land_height_m = lerp(62.0, 155.0, smooth_step(1100.0, 2300.0, landward_distance_m))

    massif_center_x_m = -1320.0
    massif_center_y_m = 1080.0
    massif_delta_x_m = world_x_m - massif_center_x_m
    massif_delta_y_m = world_y_m - massif_center_y_m
    massif_distance_m = math.hypot(massif_delta_x_m, massif_delta_y_m)
    massif_theta = math.atan2(massif_delta_y_m, massif_delta_x_m)
    massif_normalized = clamp(massif_distance_m / 28250.0, 0.0, 1.35)

    concave_shield = math.pow(clamp(1.0 - math.pow(massif_normalized, 1.42), 0.0, 1.0), 1.12)
    upper_steepening = math.pow(clamp(1.0 - massif_distance_m / 9500.0, 0.0, 1.0), 2.05) * 420.0
    shoulder_bench = math.exp(-math.pow((massif_distance_m - 17400.0) / 3300.0, 2.0)) * 185.0
    asymmetry = math.sin(world_x_m * 0.000070 + 0.9) * 142.0 + math.sin(world_y_m * 0.000060 - 1.7) * 108.0 + math.sin((world_x_m + world_y_m) * 0.000040 + 2.1) * 70.0
    coastal_protection = smooth_step(6200.0, 11800.0, landward_distance_m) * land_mask
    massif_mask = clamp(smooth_step(0.035, 0.84, 1.0 - massif_normalized) * coastal_protection, 0.0, 1.0)
    massif_height_m = (TARGET_PEAK_HEIGHT_M * concave_shield + upper_steepening + shoulder_bench + asymmetry) * coastal_protection

    def angular_delta(a: float, b: float) -> float:
        return abs(math.atan2(math.sin(a - b), math.cos(a - b)))

    def wrap_angle(a: float) -> float:
        return math.atan2(math.sin(a), math.cos(a))

    basin_angles = [-3.04, -2.73, -2.48, -2.13, -1.86, -1.57, -1.20, -0.98, -0.61, -0.33, -0.05, 0.29, 0.57, 0.92, 1.13, 1.46, 1.73, 2.03, 2.34, 2.57, 2.82, 3.03, 3.13]
    basin_widths = [0.155, 0.110, 0.135, 0.175, 0.120, 0.160, 0.105, 0.185, 0.130, 0.115, 0.170, 0.145, 0.105, 0.190, 0.100, 0.155, 0.125, 0.180, 0.112, 0.148, 0.118, 0.090, 0.140]
    basin_curves = [0.34, -0.18, 0.27, -0.31, 0.14, 0.42, -0.24, 0.19, -0.38, 0.25, -0.15, 0.33, -0.28, 0.16, -0.35, 0.22, 0.11, -0.29, 0.37, -0.12, 0.24, -0.41, 0.17]
    basin_strengths = [0.92, 0.58, 0.74, 1.00, 0.63, 0.88, 0.52, 0.94, 0.70, 0.61, 0.97, 0.79, 0.55, 1.00, 0.50, 0.83, 0.69, 0.91, 0.57, 0.77, 0.60, 0.49, 0.72]

    distance01 = clamp(massif_distance_m / 38000.0, 0.0, 1.0)
    basin_domain_warp = 0.18 * math.sin(massif_distance_m * 0.00017 + massif_theta * 2.70) + 0.11 * math.sin(world_x_m * 0.000061 - world_y_m * 0.000047 + 0.8) + 0.07 * math.sin((world_x_m - world_y_m) * 0.000039 + massif_theta * 4.1)
    warped_theta = wrap_angle(massif_theta + basin_domain_warp * smooth_step(0.08, 0.94, distance01))

    catchment_id = 0
    best_gully_delta = 999.0
    for idx, angle in enumerate(basin_angles):
        curved_center = wrap_angle(angle + basin_curves[idx] * 0.18 * smooth_step(0.16, 0.88, distance01))
        delta = angular_delta(warped_theta, curved_center)
        if delta < best_gully_delta:
            best_gully_delta = delta
            catchment_id = idx

    best_ridge_delta = 999.0
    for idx, angle in enumerate(basin_angles):
        next_idx = (idx + 1) % PRIMARY_CATCHMENT_COUNT
        boundary_b = basin_angles[next_idx] + (2.0 * math.pi if next_idx == 0 else 0.0)
        boundary_angle = wrap_angle((angle + boundary_b) * 0.5 + 0.045 * math.sin(float(idx) * 1.91))
        best_ridge_delta = min(best_ridge_delta, angular_delta(warped_theta, boundary_angle))

    basin_width = basin_widths[catchment_id]
    basin_strength = basin_strengths[catchment_id]
    mid_slope_mask = smooth_step(5200.0, 14800.0, massif_distance_m) * (1.0 - smooth_step(35000.0, 40500.0, massif_distance_m)) * land_mask
    ridge_breakup = clamp(0.72 + 0.20 * math.sin(massif_distance_m * 0.00029 + warped_theta * 5.3) + 0.14 * math.sin(world_x_m * 0.000083 + world_y_m * 0.000051), 0.38, 1.0)
    gully_breakup = clamp(0.78 + 0.18 * math.sin(massif_distance_m * 0.00034 - warped_theta * 3.7) + 0.12 * math.sin(world_x_m * 0.000047 - world_y_m * 0.000089), 0.42, 1.0)
    ridge_mask = (1.0 - smooth_step(0.042, 0.150 + basin_width * 0.18, best_ridge_delta)) * mid_slope_mask * smooth_step(5400.0, 10400.0, landward_distance_m) * ridge_breakup
    gully_reach_mask = smooth_step(6500.0, 13200.0, massif_distance_m) * smooth_step(3300.0, 7400.0, landward_distance_m) * (1.0 - smooth_step(36500.0, 43000.0, massif_distance_m)) * land_mask
    trunk_gully_mask = (1.0 - smooth_step(basin_width * 0.20, basin_width * 0.72, best_gully_delta)) * gully_reach_mask * basin_strength * gully_breakup
    branch_angle_a = wrap_angle(basin_angles[catchment_id] + basin_curves[catchment_id] * 0.42 + math.sin(distance01 * 5.0 + float(catchment_id)) * 0.10)
    branch_angle_b = wrap_angle(basin_angles[catchment_id] - basin_curves[catchment_id] * 0.35 + math.cos(distance01 * 4.0 + float(catchment_id) * 0.7) * 0.12)
    branch_reach = smooth_step(10500.0, 18000.0, massif_distance_m) * (1.0 - smooth_step(28500.0, 39000.0, massif_distance_m)) * land_mask
    secondary_branch_mask = max((1.0 - smooth_step(0.030, 0.085, angular_delta(warped_theta, branch_angle_a))) * branch_reach * 0.55, (1.0 - smooth_step(0.035, 0.095, angular_delta(warped_theta, branch_angle_b))) * branch_reach * 0.42)
    gully_mask = clamp(trunk_gully_mask + secondary_branch_mask, 0.0, 1.0)
    lahar_catchment = catchment_id in {2, 5, 8, 11, 13, 17, 20}
    lahar_corridor_mask = trunk_gully_mask * smooth_step(12200.0, 22500.0, massif_distance_m) * (1.0 - smooth_step(36000.0, 43500.0, massif_distance_m)) if lahar_catchment else 0.0
    coastal_fan_mask = (1.0 - smooth_step(basin_width * 0.30, basin_width * 0.95, best_gully_delta)) * smooth_step(1200.0, 3900.0, landward_distance_m) * (1.0 - smooth_step(7200.0, 11800.0, landward_distance_m)) * land_mask * basin_strength
    ridge_height_m = ridge_mask * lerp(85.0, 310.0, smooth_step(10500.0, 28000.0, massif_distance_m))
    gully_incision_m = gully_mask * (lerp(65.0, 260.0, smooth_step(9000.0, 33500.0, massif_distance_m)) + lahar_corridor_mask * 145.0)
    fan_deposit_m = coastal_fan_mask * lerp(22.0, 82.0, smooth_step(1900.0, 6500.0, landward_distance_m))

    crater_center_x_m = massif_center_x_m + 720.0
    crater_center_y_m = massif_center_y_m - 520.0
    crater_delta_x_m = world_x_m - crater_center_x_m
    crater_delta_y_m = world_y_m - crater_center_y_m
    crater_yaw = 0.42
    crater_u = crater_delta_x_m * math.cos(crater_yaw) + crater_delta_y_m * math.sin(crater_yaw)
    crater_v = -crater_delta_x_m * math.sin(crater_yaw) + crater_delta_y_m * math.cos(crater_yaw)
    crater_norm = math.sqrt(math.pow(crater_u / 1850.0, 2.0) + math.pow(crater_v / 1320.0, 2.0))
    crater_mask = (1.0 - smooth_step(0.72, 1.04, crater_norm)) * massif_mask
    rim_mask = ring_mask(crater_norm, 1.0, 0.27) * massif_mask
    vent_distance_m = math.hypot(world_x_m - (crater_center_x_m - 410.0), world_y_m - (crater_center_y_m + 520.0))
    vent_mask = (1.0 - smooth_step(320.0, 840.0, vent_distance_m)) * crater_mask
    breach_delta = angular_delta(warped_theta, -0.82)
    breach_mask = (1.0 - smooth_step(0.040, 0.135, breach_delta)) * smooth_step(1850.0, 4200.0, massif_distance_m) * (1.0 - smooth_step(11800.0, 17800.0, massif_distance_m)) * massif_mask
    fissure_mask = (1.0 - smooth_step(0.014, 0.045, breach_delta)) * smooth_step(5600.0, 8200.0, massif_distance_m) * (1.0 - smooth_step(18500.0, 26000.0, massif_distance_m)) * massif_mask
    broken_rim_mask = rim_mask * (1.0 - breach_mask)
    lava_crust_mask = clamp(crater_mask * 0.70 + vent_mask + breach_mask * 0.45 + fissure_mask * 0.60, 0.0, 1.0)
    unstable_crust_mask = clamp(vent_mask + fissure_mask + lava_crust_mask * 0.45, 0.0, 1.0)
    hard_blocker_mask = clamp(broken_rim_mask * 0.75 + vent_mask + fissure_mask * 0.85 + breach_mask * 0.35, 0.0, 1.0)

    long_wave_undulation_m = math.sin(world_x_m * 0.00030 + world_y_m * 0.00018) * 46.0 + math.sin(world_x_m * 0.00045 - world_y_m * 0.00027) * 32.0 + math.sin(world_x_m * 0.00012 + massif_theta * 3.0) * 24.0
    terrain_process_height_m = (
        massif_height_m
        + long_wave_undulation_m * land_mask
        + ridge_height_m
        - gully_incision_m
        + fan_deposit_m
        + broken_rim_mask * 260.0
        - (crater_mask * 420.0 + vent_mask * 260.0 + breach_mask * 190.0 + fissure_mask * 110.0)
    )
    land_height_m = max(coastal_land_height_m - gully_incision_m * 0.18, coastal_land_height_m + terrain_process_height_m)
    raw_height_m = lerp(ocean_height_m, land_height_m, land_mask)

    abs_shoreline_distance_m = abs(signed_shoreline_distance_m)
    shoreline_constraint_mask = 1.0 - smooth_step(12.0, 85.0, abs_shoreline_distance_m)
    shoreline_hard_lock_mask = 1.0 - smooth_step(76.0, 94.0, abs_shoreline_distance_m)
    shore_locked_height_m = lerp(raw_height_m, SEA_LEVEL_M, max(shoreline_constraint_mask, shoreline_hard_lock_mask))
    beach_acceptance_mask = smooth_step(20.0, 80.0, signed_shoreline_distance_m) * (1.0 - smooth_step(190.0, 260.0, signed_shoreline_distance_m)) * land_mask
    beach_accepted_height_m = clamp(shore_locked_height_m, -0.25, 9.75)
    height_m = lerp(shore_locked_height_m, beach_accepted_height_m, beach_acceptance_mask)

    return {
        "height_m": height_m,
        "signed_shoreline_distance_m": signed_shoreline_distance_m,
        "ocean_mask": 1.0 - land_mask,
        "coastal_shelf_mask": ring_mask(signed_shoreline_distance_m, -350.0, 420.0),
        "beach_ring_mask": ring_mask(signed_shoreline_distance_m, 55.0, 145.0),
        "shoreline_constraint_mask": shoreline_constraint_mask,
        "massif_mask": massif_mask,
        "ridge_mask": ridge_mask,
        "gully_mask": gully_mask,
        "crater_mask": crater_mask,
        "rim_mask": broken_rim_mask,
        "vent_mask": vent_mask,
        "breach_mask": breach_mask,
        "lava_crust_mask": lava_crust_mask,
        "unstable_crust_mask": unstable_crust_mask,
        "hard_blocker_mask": hard_blocker_mask,
        "hazard_mask": clamp(lava_crust_mask * 0.45 + unstable_crust_mask * 0.65 + hard_blocker_mask, 0.0, 1.0),
        "catchment_id": catchment_id if land_mask > 0.5 else -1,
    }


def write_png(path: Path, width: int, height: int, rows: list[bytearray]) -> None:
    raw = b"".join(b"\x00" + bytes(row) for row in rows)

    def chunk(tag: bytes, data: bytes) -> bytes:
        return struct.pack("!I", len(data)) + tag + data + struct.pack("!I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", struct.pack("!IIBBBBB", width, height, 8, 6, 0, 0, 0))
        + chunk(b"IDAT", zlib.compress(raw, 9))
        + chunk(b"IEND", b"")
    )


def rgb_to_byte(rgb: tuple[float, float, float], alpha: float = 1.0) -> bytes:
    return bytes(
        [
            int(clamp(rgb[0], 0.0, 255.0)),
            int(clamp(rgb[1], 0.0, 255.0)),
            int(clamp(rgb[2], 0.0, 255.0)),
            int(clamp(alpha * 255.0, 0.0, 255.0)),
        ]
    )


def shade_color(base: tuple[float, float, float], shade: float) -> tuple[float, float, float]:
    return tuple(channel * shade for channel in base)


def relief_color(sample: dict[str, float | int], shade: float, contour: bool) -> tuple[float, float, float]:
    h = float(sample["height_m"])
    signed = float(sample["signed_shoreline_distance_m"])
    ocean = float(sample["ocean_mask"])
    beach = float(sample["beach_ring_mask"])
    shelf = float(sample["coastal_shelf_mask"])
    crater = float(sample["crater_mask"])
    hazard = float(sample["hazard_mask"])

    if ocean > 0.5:
        depth_t = clamp((-h) / 145.0, 0.0, 1.0)
        base = lerp(185.0, 12.0, depth_t), lerp(236.0, 74.0, depth_t), lerp(235.0, 132.0, depth_t)
        if shelf > 0.2:
            base = lerp(base[0], 95.0, shelf * 0.6), lerp(base[1], 212.0, shelf * 0.6), lerp(base[2], 216.0, shelf * 0.6)
        return base

    if signed < 260.0 or beach > 0.15:
        base = (222.0, 199.0, 132.0)
    elif h < 80.0:
        base = (73.0, 139.0, 74.0)
    elif h < 350.0:
        base = (91.0, 121.0, 62.0)
    elif h < 950.0:
        base = (121.0, 92.0, 58.0)
    elif h < 1350.0:
        base = (119.0, 103.0, 88.0)
    else:
        base = (166.0, 157.0, 142.0)

    if crater > 0.2:
        base = (84.0, 72.0, 66.0)
    if hazard > 0.45:
        base = (67.0, 55.0, 52.0)
    if contour and ocean <= 0.5:
        base = tuple(channel * 0.48 for channel in base)
    return shade_color(base, shade)


def slope_degrees(h: float, hx: float, hy: float, step_m: float) -> float:
    dzdx = (hx - h) / step_m
    dzdy = (hy - h) / step_m
    return math.degrees(math.atan(math.sqrt(dzdx * dzdx + dzdy * dzdy)))


def derive_and_render(resolution: int, output_dir: Path) -> dict[str, object]:
    started = time.time()
    step_m = WORLD_SIZE_M / float(resolution - 1)
    light = (-0.45, -0.55, 0.70)
    light_len = math.sqrt(sum(component * component for component in light))
    light = tuple(component / light_len for component in light)

    color_rows: list[bytearray] = []
    height_rows: list[bytearray] = []
    slope_rows: list[bytearray] = []
    mask_rows: list[bytearray] = []

    min_h = float("inf")
    max_h = float("-inf")
    max_slope = 0.0
    peak_xy = (0.0, 0.0)
    shore_error = 0.0
    beach_samples = 0
    beach_pass = 0
    ocean_samples = 0
    ocean_below = 0
    square_edge_samples = 0
    square_edge_violations = 0

    for py in range(resolution):
        y = HALF_EXTENT_M - float(py) * step_m
        color_row = bytearray()
        height_row = bytearray()
        slope_row = bytearray()
        mask_row = bytearray()

        for px in range(resolution):
            x = -HALF_EXTENT_M + float(px) * step_m
            s0 = sample_terrain_m(x, y)
            sx = sample_terrain_m(min(x + step_m, HALF_EXTENT_M), y)
            sy = sample_terrain_m(x, max(y - step_m, -HALF_EXTENT_M))
            h = float(s0["height_m"])
            hx = float(sx["height_m"])
            hy = float(sy["height_m"])
            sl = slope_degrees(h, hx, hy, step_m)
            max_slope = max(max_slope, sl)

            dzdx = (hx - h) / step_m
            dzdy = (hy - h) / step_m
            normal = (-dzdx, dzdy, 1.0)
            normal_len = math.sqrt(sum(component * component for component in normal))
            normal = tuple(component / normal_len for component in normal)
            diffuse = max(0.0, sum(normal[i] * light[i] for i in range(3)))
            shade = 0.55 + 0.55 * diffuse

            contour = False
            if h > SEA_LEVEL_M:
                contour = int(math.floor(h / 100.0)) != int(math.floor(hx / 100.0)) or int(math.floor(h / 100.0)) != int(math.floor(hy / 100.0))
            coast_line = abs(float(s0["signed_shoreline_distance_m"])) <= max(16.0, step_m * 0.65)

            if h < min_h:
                min_h = h
            if h > max_h:
                max_h = h
                peak_xy = (x, y)
            if float(s0["shoreline_constraint_mask"]) > 0.1:
                shore_error = max(shore_error, abs(h - SEA_LEVEL_M))
            signed = float(s0["signed_shoreline_distance_m"])
            if 20.0 <= signed <= 190.0:
                beach_samples += 1
                if -0.25 <= h <= 10.0:
                    beach_pass += 1
            if float(s0["ocean_mask"]) > 0.5:
                ocean_samples += 1
                if h <= SEA_LEVEL_M:
                    ocean_below += 1
            if px == 0 or py == 0 or px == resolution - 1 or py == resolution - 1:
                square_edge_samples += 1
                if h > SEA_LEVEL_M - 0.5:
                    square_edge_violations += 1

            color = relief_color(s0, shade, contour)
            if coast_line:
                color = (240.0, 238.0, 211.0)
            color_row.extend(rgb_to_byte(color))

            h_norm = clamp((h + 145.0) / (4500.0 + 145.0), 0.0, 1.0)
            height_row.extend(rgb_to_byte((h_norm * 255.0, h_norm * 255.0, h_norm * 255.0)))
            s_norm = clamp(sl / 55.0, 0.0, 1.0)
            slope_row.extend(rgb_to_byte((s_norm * 255.0, s_norm * 255.0, s_norm * 255.0)))
            mask_row.extend(
                rgb_to_byte(
                    (
                        float(s0["beach_ring_mask"]) * 255.0,
                        max(float(s0["ridge_mask"]), float(s0["gully_mask"])) * 255.0,
                        float(s0["hazard_mask"]) * 255.0,
                    )
                )
            )

        color_rows.append(color_row)
        height_rows.append(height_row)
        slope_rows.append(slope_row)
        mask_rows.append(mask_row)

    prefix = f"PR5_Batch004_X6_Island_{resolution}px"
    outputs = {
        "color_relief_png": output_dir / f"{prefix}_ColorRelief.png",
        "height_png": output_dir / f"{prefix}_Height.png",
        "slope_png": output_dir / f"{prefix}_Slope.png",
        "mask_atlas_png": output_dir / f"{prefix}_MaskAtlas_BeachR_RidgeGullyG_HazardB.png",
    }
    write_png(outputs["color_relief_png"], resolution, resolution, color_rows)
    write_png(outputs["height_png"], resolution, resolution, height_rows)
    write_png(outputs["slope_png"], resolution, resolution, slope_rows)
    write_png(outputs["mask_atlas_png"], resolution, resolution, mask_rows)

    elapsed = time.time() - started
    beach_pct = 100.0 * beach_pass / beach_samples if beach_samples else 0.0
    ocean_pct = 100.0 * ocean_below / ocean_samples if ocean_samples else 0.0
    manifest = {
        "version": VERSION,
        "resolution_px": resolution,
        "world_size_m": WORLD_SIZE_M,
        "pixel_spacing_m": step_m,
        "height_min_m": round(min_h, 4),
        "height_max_m": round(max_h, 4),
        "peak_xy_m": [round(peak_xy[0], 3), round(peak_xy[1], 3)],
        "shoreline_error_max_m": round(shore_error, 4),
        "beach_continuity_pct": round(beach_pct, 3),
        "ocean_below_sea_pct": round(ocean_pct, 3),
        "square_edge_ocean_violations": square_edge_violations,
        "square_edge_samples": square_edge_samples,
        "slope_max_deg_preview": round(max_slope, 4),
        "elapsed_seconds": round(elapsed, 3),
        "linear_scale_multiplier_vs_batch003": 6.0,
        "area_scale_multiplier_vs_batch003": 36.0,
        "morphology": "x6-irregular-basin-graph-anti-radial",
        "outputs": {key: str(path.as_posix()) for key, path in outputs.items()},
        "note": "Tracked preview evidence generated from deterministic PR5 Batch 004 x6 anti-radial terrain math. Not an AI-generated image and not full 8129x8129 export.",
    }
    manifest_path = output_dir / f"{prefix}_Manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    outputs["manifest_json"] = manifest_path

    readme = output_dir / "README.md"
    readme.write_text(
        "# PR5 Batch 004 X6 Terrain Preview Images\n\n"
        "These files are labelled, tracked terrain evidence generated from the deterministic PR5 Batch 004 x6 anti-radial terrain model. "
        "They are not AI art, prompt renders, broad documentation, or UE screenshots.\n\n"
        f"Latest generated preview: `{prefix}`.\n\n"
        "## Files\n\n"
        f"- `{outputs['color_relief_png'].name}` — island color relief view with hillshade, coastline, beach, shelf, ridges, crater, and volcanic relief.\n"
        f"- `{outputs['height_png'].name}` — grayscale sampled height image.\n"
        f"- `{outputs['slope_png'].name}` — grayscale preview slope image.\n"
        f"- `{outputs['mask_atlas_png'].name}` — RGB mask atlas: red = beach, green = ridge/gully, blue = hazard.\n"
        f"- `{manifest_path.name}` — metrics and provenance for the generated preview.\n\n"
        "## Regenerate\n\n"
        "```bash\n"
        f"python3 scripts/terrain-preview-export.py --resolution {resolution} --output-dir Images/TerrainPreview\n"
        "```\n\n"
        "Full 8129 x 8129 export at 12 m/sample remains a later heavyweight export path.\n",
        encoding="utf-8",
    )
    outputs["readme_md"] = readme
    return manifest


def main() -> int:
    parser = argparse.ArgumentParser(description="Export deterministic PR5 Batch 004 x6 anti-radial island preview PNGs.")
    parser.add_argument("--resolution", type=int, default=1024, help="Preview resolution per side. Default: 1024.")
    parser.add_argument("--output-dir", type=Path, default=Path("Images/TerrainPreview"), help="Output directory for tracked terrain preview evidence.")
    args = parser.parse_args()

    if args.resolution < 64:
        raise SystemExit("resolution must be >= 64")
    manifest = derive_and_render(args.resolution, args.output_dir)
    print(json.dumps(manifest, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
