
module base() {
    cylinder(d=8, h=17, $fn=190);
    translate([0, 85, 0]) {
        cylinder(d=8, h=17, $fn=190);
    }
    translate([-4, 1.5, 0]) {
        cube([8, 85 - 1.5, 2]);
    }
    difference() {
        translate([0, 85/2 - 15, 0]) {
            cube([10, 30, 15]);
        }
    }
}


difference() {
    base();
    cylinder(d=3.3, h=20, $fn=190);
    translate([0, 85, 0]) {
        cylinder(d=3.3, h=20, $fn=190);
    }
}

module spacing_screws_back() {
    difference() {
        cylinder(d=8, h=17, $fn=190);
        cylinder(d=3.3, h=20, $fn=190);
    }
    translate([0, 85, 0]) {
        difference() {
            cylinder(d=8, h=17, $fn=190);
            cylinder(d=3.3, h=20, $fn=190);
        }
    }
}