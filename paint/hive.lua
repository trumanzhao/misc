--绘制蜂巢标志: 三个六边形

require("gdi");

polygon_size = polygon_size or 64;

function make_polygon(arc, cx, cy)
    local polygon = {fill_points={}, edge_points={}};
    local fill_points = polygon.fill_points;
    local edge_points = polygon.edge_points;

    local delta = math.rad(60);

    arc = math.rad(arc);
    for i = 0, 5 do
        fill_points[#fill_points + 1] =
        {
            x=cx + polygon_size * math.cos(arc + delta * i),
            y=cy + polygon_size * math.sin(arc + delta * i)
        };

        local edge_size = polygon_size + 1;
        edge_points[#edge_points + 1] =
        {
            x=cx + edge_size * math.cos(arc + delta * i),
            y=cy + edge_size * math.sin(arc + delta * i)
        };
    end
    return polygon;
end

centerx = centerx or 500;
centery = centery or 500;
function window.on_size(w, h)
    centerx = w / 2;
    centery = h / 2;
    build_polygons();
end

polygons = {};
rotate_base = rotate_base or 210; --第一个六边形中心的偏离X轴的角度
function build_polygons()
    polygons = {};
    local polygon_colors = {0xff770000, 0xff007700, 0xff000077};
    for i = 1, 3 do
        local distance = polygon_size + 2; --六边形中心到世界中心的距离
        local center_agc = math.rad(rotate_base) + math.rad(120) * (i - 1);
        local cx = centerx + distance * math.cos(center_agc);
        local cy = centery + distance * math.sin(center_agc);
        local polygon = make_polygon(rotate_base, cx, cy);
        polygon.color = polygon_colors[i];
        polygons[#polygons + 1] = polygon;
    end
end

build_polygons();

--window.set_back_color(0xff777777);
window.set_back_color(0xff000000);
function window.on_draw()
    window.set_smoothing_mode(SmoothingMode.SmoothingModeAntiAlias);
    window.set_line_width(2);
    window.set_line_color(0xffcccccc);

    for idx, polygon in ipairs(polygons) do
        window.set_fill_color(polygon.color);
        window.fill_polygon(polygon.fill_points);
        --window.draw_polygon(polygon.edge_points);
    end
end

function window.on_timer()
    rotate_base = (rotate_base + 1) % 360;

    local rad = math.rad(rotate_base);
    polygon_size = 64 + math.sin(rad) * 32;
    build_polygons();
end
