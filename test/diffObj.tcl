#/usr/bin/env tclsh
source readObj.tcl

# compare floats with tolerance
proc almostEqual {f1 f2 {tol .01}} {
	return [expr {abs($f1 - $f2)/double(min($f1, $f2)) < $tol}]
}

proc translateVert {arrayName point} {
	upvar $arrayName o2v
	if {[array names o2v $point] eq [list $point]} {
		return $o2v($point)
	} else {
		puts "No vertex found in $arrayName matching $point"
		return ""
	}
}

proc checkTriangle {arrayName t} {
	upvar $arrayName o2tri
	if {[array names o2tri $t] eq [list $t]} {
		array unset o2tri $t
		return 1
	}
	return 0
}

# compare two 3d model .obj files
proc diffObj {f1 f2} {
	set o1 [readObj $f1]
	set o2 [readObj $f2]

	# must have same number of vertices and faces
	if {[llength [dict get $o1 "verts"]] != [llength [dict get $o2 "verts"]]} {
		return 1
	}
	if {[llength [dict get $o1 "faces"]] != [llength [dict get $o2 "faces"]]} {
		return 1
	}

	# compare faces by resolving the vert indices (in case verts are in a different order)
	set verts1 [dict get $o1 "verts"]
	set verts2 [dict get $o2 "verts"]

	## for now, assume floats will match exactly (will be hard going forward...)
	## should be able to walk the o2v map and look for close matches...

	# build a map from o1 vert index to o2 vert index
	set i 0
	foreach v $verts2 {
		set o2v($v) $i
		incr i
	}

	set ret 0
	# build a set of all the triangles in object 2
	foreach t [dict get $o2 "faces"] {
		# pull indices
		set v0 [lindex $t 0]
		set v1 [lindex $t 1]
		set v2 [lindex $t 2]

		set tt "$v0 $v1 $v2"
		set o2tri($tt) 1
	}

	# foreach triangle in object 1
	foreach t [dict get $o1 "faces"] {
		set v0 [lindex $t 0]
		set v1 [lindex $t 1]
		set v2 [lindex $t 2]

		# translate index for o2
		set p0 [lindex $verts1 $v0]
		set pt0 [translateVert o2v $p0]
		if {$pt0 eq ""} {
			return -1
		}

		set p1 [lindex $verts1 $v1]
		set pt1 [translateVert o2v $p1]
		if {$pt1 eq ""} {
			return -1
		}

		set p2 [lindex $verts1 $v2]
		set pt2 [translateVert o2v $p2]
		if {$pt2 eq ""} {
			return -1
		}

		# need to find in o2tri one of:
		# $pt0 $pt1 $pt2
		# $pt1 $pt2 $pt0
		# $pt2 $pt0 $pt1
		# (assumes winding order is still good)
		if {[checkTriangle o2tri "$pt0 $pt1 $pt2"]} {
			continue
		}
		if {[checkTriangle o2tri "$pt1 $pt2 $pt0"]} {
			continue
		}
		if {[checkTriangle o2tri "$pt2 $pt0 $pt1"]} {
			continue
		}

		puts "No match for o1 triangle $t ($p0 $p1 $p2)"
		set ret 1
	}

	foreach n [array names o2tri] {
		puts "Unmatched o2 triangle $n"
		set ret 1
	}

	return $ret
}

if {$argc < 2} {
	puts "Usage: $argv0 <file1.obj> <file2.obj>"
} else {
	set f1 [lindex $argv 0]
	set f2 [lindex $argv 1]
	puts [diffObj $f1 $f2]
}

