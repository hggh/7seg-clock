


difference() {
    cube([10, 20, 2]);
    translate([5, 6.25, -1]) {
        cylinder(d=3.2, h=10, $fn=190);
    }
}
translate([-35, 20 - 5, 0]) {
    cube([80, 5, 15]);
}
difference() {
    translate([-35, -7, 0]) {
        cube([10, 22, 2]);
    }
    translate([-29.528, -1.826, -1]) {
        cylinder(d=3.2, h=10, $fn=190);
    }
}

difference() {
    translate([25 + 10, -7, 0]) {
        cube([10, 22, 2]);
    }
    translate([29.528 +10, -1.826, -1]) {
        cylinder(d=3.2, h=10, $fn=190);
    }
}