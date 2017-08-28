--绘制坐标系网格

require("gdi");

_G.print = window.log_txt;


window_width = window_width or 800;
window_height = window_height or 800;

window.set_log_back_color(0x77000000);
window.set_log_edge_color(0xFFCCCCCC);

grid_width = 40;
grid_height = 30;

window.log_clear();

function window.on_size(w, h)
    window_width = w;
    window_height = h;
    window.set_text_box(w / 4, h * 3 / 4, w / 2, h / 2);
end

window.fill_rect = function(x, y, w, h, color)
    local pts = {{x, y}, {x, y-h}, {x+w, y-h}, {x+w, y}};
    window.set_fill_color(color or 0xffffffff);
    window.fill_polygon(pts);
end

window.set_back_color(0xff000000);

function window.on_draw()
    window.set_smoothing_mode(SmoothingMode.SmoothingModeHighSpeed);
    window.set_line_width(1);
    window.set_line_color(0xff337733);
    window.set_dash_style(DashStyle.DashStyleDashDotDot);

    for y = 0, window_height // grid_height do
        window.draw_line({x=0,y=grid_height*y}, {x=window_width, y=grid_height*y});
    end

    for x = 0, window_width // grid_width do
        window.draw_line({x=grid_width*x,y=0}, {x=grid_width*x, y=window_height});
    end

    window.set_smoothing_mode(SmoothingMode.SmoothingModeHighQuality);

    if src_point and dst_point then
        window.set_dash_style(DashStyle.DashStyleDot);
        window.set_line_color(0xFF777777);
        window.draw_line(src_point, dst_point);
        if now_point then
            local next_point = ray_cast(src_point.x, src_point.y, now_point.x, now_point.y, grid_width, grid_height);
            while next_point do
                local x, y = next_point();
                if x == nil then
                    break;
                end
                x = x * grid_width + 1;
                y = y * grid_height + grid_height - 1;
                window.fill_rect(x, y, grid_width - 2, grid_height - 2, 0x3300ff00);
            end

            window.set_line_width(2);
            window.set_dash_style(DashStyle.DashStyleSolid);
            window.set_line_color(0xFF009900);
            window.draw_line(src_point, now_point);
        end
    end

    if src_point then
        draw_cross(src_point.x, src_point.y, 0xffffff00);
    end

    if dst_point then
        draw_cross(dst_point.x, dst_point.y, 0xff00ff00);
    end
end

cross_size = 5;
function draw_cross(x, y, color, width)
    window.set_line_width(width or 1);
    window.set_line_color(color or 0xffffffff);
    window.set_dash_style(DashStyle.DashStyleSolid);
    window.draw_line({x-cross_size, y-cross_size}, {x+cross_size, y+cross_size});
    window.draw_line({x-cross_size, y+cross_size}, {x+cross_size, y-cross_size});
end

function window.on_timer()
    if src_point and dst_point and progress < max_progress then
        local x = src_point.x * (max_progress-progress) + dst_point.x * progress;
        local y = src_point.y * (max_progress-progress) + dst_point.y * progress;
        now_point = {x=x//max_progress, y=y//max_progress};
        progress = progress + 3;
    end
end

--progress = 0;

--window.log_txt(progress);

--步进进度,精度max_progress
progress = progress or 0;
max_progress = 1000;

dst_point = {x=640, y=150};
src_point = {x=640, y=600};
progress = 0;

function window.on_lbutton_up(x, y)
    dst_point = {x=x, y=y};
    progress = 0;
    print("dst: x="..x..", y="..y);
end

function window.on_rbutton_up(x, y)
    src_point = {x=x, y=y};
    progress = 0;
    print("src: x="..x..", y="..y);
end

--计算射线穿过的网格,返回迭代器
function ray_cast(src_x, src_y, dst_x, dst_y, size_x, size_y)
    function point_to_grid(point, grid_size, point_step)
        local grid = (point_step >= 0 and point or point - 1) // grid_size;
        return point >= 0 and grid or grid - 1;
    end

    local step_x = dst_x >= src_x and 1 or -1;
    local step_y = dst_y >= src_y and 1 or -1;
    local grid_src_x = point_to_grid(src_x, size_x, step_x);
    local grid_src_y = point_to_grid(src_y, size_y, step_y);
    local grid_dst_x = point_to_grid(dst_x, size_x, step_x);
    local grid_dst_y = point_to_grid(dst_y, size_y, step_y);

    function intersect(grid_x, grid_y)
        local delta_x = dst_x - src_x;
        local delta_y = dst_y - src_y;

        if delta_x == 0 then
            return (grid_y - grid_src_y) * (grid_y - grid_dst_y) <= 0;
        end

        if delta_y == 0 then
            return (grid_x - grid_src_x) * (grid_x - grid_dst_x) <= 0;
        end

        local x1 = size_x * grid_x;
        local x2 = x1 + size_x;
        local y1 = size_y * grid_y;
        local y2 = y1 + size_y;
        local ya = src_y * delta_x + delta_y * (x1 - src_x);
        local yb = src_y * delta_x + delta_y * (x2 - src_x);
        if delta_x > 0 then
            return not (ya < y1 * delta_x and yb < y1 * delta_x or ya > y2 * delta_x and yb > y2 * delta_x);
        end
        return not (ya > y1 * delta_x and yb > y1 * delta_x or ya < y2 * delta_x and yb < y2 * delta_x);
    end

    local next_x = grid_src_x;
    local next_y = grid_src_y - step_y;
    local optimize_y = grid_src_y;
    return function()
        next_y = next_y + step_y;
        if step_y * (next_y - grid_dst_y) <= 0 and intersect(next_x, next_y) then
            return next_x, next_y;
        end

        next_x = next_x + step_x;
        if step_x * (next_x - grid_dst_x) > 0 then
            return nil, nil;
        end

        next_y = optimize_y;
        while not intersect(next_x, next_y) do
            next_y = next_y + step_y;
        end
        optimize_y = next_y;
        return next_x, next_y;
    end
end

--[[
local it = ray_cast(200, 600, 200, 100, 40, 30);

while it do
x, y = it();
print("x="..tostring(x)..", y="..tostring(y));
if x == nil then
    break;
end
end
--]]



