# return the dot product of v1 and v2
proc dot {v1 v2} {
	set v11 [lindex $v1 0]
	set v12 [lindex $v1 1]
	set v13 [lindex $v1 2]

	set v21 [lindex $v2 0]
	set v22 [lindex $v2 1]
	set v23 [lindex $v2 2]

	return [expr {$v11 * $v21 + $v12 * $v22 + $v13 * $v23}]
}

# return the cross product of v1 and v2
proc cross {v1 v2} {
	set v11 [lindex $v1 0]
	set v12 [lindex $v1 1]
	set v13 [lindex $v1 2]

	set v21 [lindex $v2 0]
	set v22 [lindex $v2 1]
	set v23 [lindex $v2 2]

	set o1 [expr {$v12 * $v23 - $v13 * $v22}]
	set o2 [expr {$v13 * $v21 - $v11 * $v23}]
	set o3 [expr {$v11 * $v22 - $v12 * $v21}]

	return [list $o1 $o2 $o3]
}

# return the length of the vector
proc vlength {v} {
	set v1 [lindex $v 0]
	set v2 [lindex $v 1]
	set v3 [lindex $v 2]

	return [expr {sqrt($v1 * $v1 + $v2 * $v2 + $v3 * $v3)}]
}

# return the normalized vector
proc vnormalized {v} {
	set v1 [lindex $v 0]
	set v2 [lindex $v 1]
	set v3 [lindex $v 2]

	set l [expr {sqrt($v1 * $v1 + $v2 * $v2 + $v3 * $v3)}]
	if {$l == 0} {
		return [list 0 0 0]
	}

	set o1 [expr {$v1 / $l}]
	set o2 [expr {$v2 / $l}]
	set o3 [expr {$v3 / $l}]
	return [list $o1 $o2 $o3]
}

