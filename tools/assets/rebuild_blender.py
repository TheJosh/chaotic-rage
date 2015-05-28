# Export as a .dae
#
# Run as follows:
#   blender -b my.blend -P rebuild_blender.py
#
import bpy
import sys
import os


def include(filename):
	if os.path.exists(filename):
		with open(filename) as f:
			code = compile(f.read(), filename, 'exec')
			exec(code, globals())
	else:
		raise Exception('Unable to find ' + filename)

class CliOperator (bpy.types.Operator):
	def report(type, msg):
		print(type, msg, file=sys.stderr)


include('tools/assets/export_dae.py')

basename = bpy.path.basename(bpy.data.filepath)
destname = "data-test/cr/models/" + basename.replace(".blend", ".dae")
opts = {};


# What to export (options -- EMPTY, CAMERA, LAMP, ARMATURE, MESH, CURVE)
opts["object_types"] = {'ARMATURE', 'MESH'}

# Export only selected objects (and visible in active layers if that applies).
opts["use_export_selected"] = False

# Apply modifiers to mesh objects (on a copy!).
opts["use_mesh_modifiers"] = True

# Export Tangent and Binormal arrays (for normalmapping).
opts["use_tangent_arrays"] = True

# Export Triangles instead of Polygons.
opts["use_triangles"] = True

# Copy Images (create images/ subfolder)
opts["use_copy_images"] = True

# Image path (default is images/)
opts["image_path"] = ''

# Export only objects on the active layers.
opts["use_active_layers"] = False

# Exclude skeleton bones with names that begin with 'ctrl'.
opts["use_exclude_ctrl_bones"] = True

# Export keyframe animation
opts["use_anim"] = True

# Export all actions for the first armature found in separate DAE files
opts["use_anim_action_all"] = False

# Skip exporting of actions whose name end in (-noexp). Useful to skip control animations.
opts["use_anim_skip_noexp"] = False

# Remove double keyframes
opts["use_anim_optimize"] = False

# Tolerence for comparing double keyframes (higher for greater accuracy)
opts["anim_optimize_precision"] = 6.0

# Use Metadata
opts["use_metadata"] = True


exp = DaeExporter(destname, opts, CliOperator)
exp.export()

