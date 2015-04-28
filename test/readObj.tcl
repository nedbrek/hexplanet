# read a 3d model from a .obj file
# returns a dictionary of the vertices and triangles (faces)
proc readObj {filename} {
	set verts [list]
	set faces [list]

	set f [open $filename]

	# foreach line in the file
	set l [gets $f]
	while {![eof $f]} {

		set cmd [lindex $l 0]
		if {$cmd eq "v"} {
			# vertex (three floats)
			lappend verts [lrange $l 1 3]
		} elseif {$cmd eq "f"} {
			# face (triangle) (three indices)
			set fTmp [list]
			foreach item [lrange $l 1 end] {
				# faces indices start at 1, adjust to 0 base
				lappend fTmp [expr [lindex [split $item "/"] 0] - 1]
			}
			lappend faces $fTmp
		}

		set l [gets $f]
	}
	close $f

	return [dict create verts $verts faces $faces]
}

