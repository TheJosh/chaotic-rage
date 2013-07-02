bl_info = {
	"name": "Chaotic Rage map (.conf)",
	"author": "Josh Heidenreich",
	"version": (1, 0),
	"blender": (2, 5, 7),
	"api": 35622,
	"location": "File > Export",
	"description": "Export Model to Chaotic Rage map file",
	"warning": "",
	"wiki_url": "",
	"tracker_url": "",
	"category": "Import-Export"}

'''
Usage Notes:


'''

import bpy
from bpy.props import *
import bpy_extras
from bpy_extras.io_utils import ExportHelper 
import time
import os


######  DOES EXPORT  #######
def do_export(context, props, filepath):
	out = open(filepath, "w")
	
	# Determine size
	sizeX = 200
	sizeZ = 200
	for obj in bpy.data.objects:
		if obj.name == 'heightmap':
			sizeX = int(obj.dimensions[0])
			sizeZ = int(obj.dimensions[1])
	
	# Write details to the file
	out.write("width=" + str(sizeX) + "\n")
	out.write("height=" + str(sizeZ) + "\n")
	
	# Heightmap
	out.write("\n")
	out.write("heightmap {  texture=\"terrain.png\"  data=\"heightmap.png\"  }\n")
	
	# TODO: have a way to choose if this is exported or not
	out.write("\n")
	out.write("skybox  {  base=\"skybox_\"  ext=\".jpg\"  }\n")
	
	# TODO: have a way to choose if this is exported or not
	out.write("\n")
	out.write("water {  texture=\"water.png\"  }\n")
	
	# We have just one mesh, which we export later
	out.write("\n")
	out.write("mesh {  model=\"scene.dae\"  pos={ " + str(sizeX/2) + ", 0.0, " + str(sizeZ/2) + " }  }\n")
	
	# Also need a zone
	out.write("\n")
	out.write("zone {  width = 2  height = 2  x = 20  y = 20  angle = 0  spawn = {0, 1, 2}  }\n")
	
	# just for debugging
	out.write("\n")
	for obj in bpy.data.objects:
		out.write("# " + obj.type + ": " + obj.name + "\n")
	
	out.close()
	
	# Only select meshes, and exclude some stuff (heightmap plane)
	for obj in bpy.data.objects:
		obj.select = (obj.type == 'MESH')
		
		if obj.name == 'heightmap':
			obj.select = False

	# Export the model in collada format, plus images
	os.path.dirname(filepath) + "models/"
	bpy.ops.wm.collada_export(
		filepath=os.path.dirname(filepath) + "/models/scene.dae",
		check_existing=False, selected=True, apply_modifiers=True, active_uv_only=False,
		include_uv_textures=True, include_material_textures=True, use_texture_copies=True,
		include_armatures=False, include_shapekeys=False, include_children=True)

	return True



######  EXPORT OPERATOR  #######
class Export_cr_map(bpy.types.Operator, ExportHelper):
	'''Exports the scene as a Chaotic Rage map'''
	bl_idname = "export_cr_map.conf"
	bl_label = "Export CR map"

	filename_ext = ".conf"
	
	apply_modifiers = BoolProperty(name="Apply Modifiers",
							description="Applies the Modifiers",
							default=True)

	
	@classmethod
	def poll(cls, context):
		return context.active_object.type in ['MESH', 'CURVE', 'SURFACE', 'FONT']

	def execute(self, context):
		start_time = time.time()
		print('\n_____START_____')
		props = self.properties
		filepath = self.filepath
		filepath = bpy.path.ensure_ext(filepath, self.filename_ext)

		exported = do_export(context, props, filepath)
		
		if exported:
			print('finished export in %s seconds' %((time.time() - start_time)))
			print(filepath)
			
		return {'FINISHED'}

	def invoke(self, context, event):
		wm = context.window_manager

		if True:
			# File selector
			wm.fileselect_add(self) # will run self.execute()
			return {'RUNNING_MODAL'}
		elif True:
			# search the enum
			wm.invoke_search_popup(self)
			return {'RUNNING_MODAL'}
		elif False:
			# Redo popup
			return wm.invoke_props_popup(self, event) #
		elif False:
			return self.execute(context)


### REGISTER ###

def menu_func(self, context):
	self.layout.operator(Export_cr_map.bl_idname, text="Chaotic Rage map (.conf)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
	register()
