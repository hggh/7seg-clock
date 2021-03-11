module make_part_of_seg() {
    points = [
        // outer
        [0, -1.2 - 2],
        [-3.925 - 1.2, 3.925],
        [-3.925 - 1.2, 31.440],
        [0, 35.37 + 1.2 + 2],
        [3.925 + 1.2, 31.440],
        [3.925 + 1.2, 3.925],
        // inner
        [0,0],
        [-3.925, 3.925],
        [-3.925, 31.440],
        [0, 35.37],
        [3.925, 31.440],
        [3.925, 3.925]
    ];

    paths = [[0, 1, 2, 3, 4, 5], [6, 7, 8, 9, 10, 11]];
    linear_extrude(10) {
        polygon(points, paths, 10);
    }
}

module make_point() {
    // trenner hour : minutes
    difference() {
        cylinder(d=7.850 + 2.5, h=10, $fn=190);
        translate([0, 0, -1]) cylinder(d=7.850, h=30, $fn=190);
    }
}

module make_middle() {
    difference() {
        cube([10, 50, 1.2]);
        translate([5, 25, -1]) cylinder(d=3.1, h=10, $fn=190);
        translate([5, 7.03, -1]) cylinder(d=7.850, h=30, $fn=190);
        translate([5, 42.968, -1]) cylinder(d=7.850, h=30, $fn=190);
    }
    translate([5, 7.03, 0]) make_point();
    translate([5, 42.968, 0]) make_point(); 
}

module make_seg() {
    // left
    make_part_of_seg();
    translate([0, 39.6, 0]) make_part_of_seg();
    // top
    translate([2.116, 77.081, 0]) rotate([0, 0,270]) make_part_of_seg();
    // middle
    translate([2.116, 37.482, 0]) rotate([0, 0,270]) make_part_of_seg();
    // down
    translate([2.116, - 2.117, 0]) rotate([0, 0,270]) make_part_of_seg();
    // right
    translate([39.6, 0, 0]) make_part_of_seg();
    translate([39.6, 39.6, 0]) make_part_of_seg();
    
    // top holder
    translate([14.799, 63.156]) {
        difference() {
            cube([10, 10, 1.2]);
            translate([5, 5, -1]) cylinder(d=3.1, h=10, $fn=190);
        }
    }   
    // down holder
    translate([14.799, 1.809]) {
        difference() {
            cube([10, 10, 1.2]);
            translate([5, 5, -1]) cylinder(d=3.1, h=10, $fn=190);
        }
    }
}

module spacing_screws() {
    difference() {
        cylinder(d=7, h=10, $fn=190);
        cylinder(d=3.1, h=12, $fn=190);
    }
}

make_seg();

translate([-60, 0, 0]) {
    make_middle();
}

translate([-80, 0, 0]) {
    spacing_screws();
}