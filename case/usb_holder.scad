use <hggh/mico_usb_breakout_holder_green.scad>


module back() {
    difference() {
        cube([1.2, 24, 14]);
        translate([-1, (24-14.6-4)/2, (14-7)/2]) cube([3, 14.6+4, 8.4]);
    }
}

difference() {
    cube([3, 20, 10]);
    translate([-1, (20-14)/2, (10-8)/2]) cube([5, 14, 8]);
    translate([2, (20-14)/2, 8]) cube([2, 14, 2]);
}
translate([1, (20 - 14.6 -4)/2, 1]) mico_usb_breakout_holder_green();
translate([3, -2, -2]) back();
translate([3, 0, -2]) cube([15, 19.3, 3.6]);