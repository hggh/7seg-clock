


points = [
        [0, 0],
        [0, 17.75],
        [35, 5],
        [58.719, 70.199],
        [63.485, 70.199],
        [63.48, 0]
];
paths = [[0, 1, 2, 3, 4, 5]];


difference() {
    linear_extrude(3.7) {
        polygon(points, paths);
    }
    translate([10, 2.5, 1.2]) cube([3, 3, 5]);
    translate([50.485, 2.5, 1.2]) cube([3, 3, 5]);
    translate([50.485, 16.5, 1.2]) cube([3, 3, 5]);
    translate([50.485, 30.5, 1.2]) cube([3, 3, 5]);
}

translate([90, 0, 0]) {
    cube([100, 2.9, 2.9]);
}

translate([200, 0, 0]) {
    difference() {
        linear_extrude(3.7) {
            polygon(points, paths);
        }
        translate([10, 2.5, 0]) cube([3, 3, 2.5]);
        translate([50.485, 2.5, 0]) cube([3, 3, 2.5]);
        translate([50.485, 16.5, 0]) cube([3, 3, 2.5]);
        translate([50.485, 30.5, 0]) cube([3, 3, 2.5]);
    }
}
