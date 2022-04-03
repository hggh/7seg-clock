$fn=190;


translate([-2, -3, 0])
    cube([30, 6, 2]);
cylinder(d=8, h=6);
cylinder(d=3, h=9);
translate([28, 0, 0]) {
    cylinder(d=8, h=6);
    cylinder(d=3, h=9);
}