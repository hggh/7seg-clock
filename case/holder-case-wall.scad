
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
        translate([0, 85/2, 10]) {
            translate([0, 10, 0]) {
                rotate([0, 90, 0 ]) {
                    cylinder(r=5.5 / 2 / cos(180 / 6) + 0.05, h=5, $fn=6);
                    cylinder(d=3.5, h=30, $fn=190);
                }
            }
            translate([0, -10, 0]) {
                rotate([0, 90, 0 ]) {
                    cylinder(r=5.5 / 2 / cos(180 / 6) + 0.05, h=5, $fn=6);
                    cylinder(d=3.5, h=30, $fn=190);
                }
            }
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