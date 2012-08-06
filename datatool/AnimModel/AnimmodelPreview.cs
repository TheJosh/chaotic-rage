using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SharpGL;
using SharpGL.Serialization.Wavefront;
using SharpGL.SceneGraph;
using SharpGL.SceneGraph.Assets;

namespace datatool
{
    class AnimmodelPreview : PreviewBase
    {
        private Scene s;
        private Texture tx;


        /**
         * Init display of a given object
         **/
        public override void Init(OpenGL gl, base_item item)
        {
            if (!(item is animmodel_item)) return;
            animmodel_item ii = (animmodel_item)item;

            if (ii.Mesh == null || ii.Mesh == "") {
                s = null;
                return;
            }

            string tmp = System.IO.Path.GetTempFileName();
            System.IO.File.WriteAllText(tmp, Program.dp.readFile("animmodels\\" + ii.Mesh + ".obj"));

            ObjFileFormat fmt = new ObjFileFormat();
            s = fmt.LoadData(tmp);
            s.RenderBoundingVolumes = false;
            
            System.IO.File.Delete(tmp);

            if (ii.Texture != null) {
                tmp = System.IO.Path.GetTempFileName();
                System.IO.File.WriteAllBytes(tmp, Program.dp.readBinaryFile("animmodels\\" + ii.Texture + ".png"));

                tx = new SharpGL.SceneGraph.Assets.Texture();
                tx.Create(gl, tmp);

                System.IO.File.Delete(tmp);
            } else {
                tx = null;
            }
        }


        /**
         * Render
         **/
        public override void Render(OpenGL gl)
        {
            if (s == null) return;

            if (tx != null) {
                tx.Bind(gl);
            }

            s.OpenGL = gl;
            s.RenderElement(s.SceneContainer.Children[0], SharpGL.SceneGraph.Core.RenderMode.Render);
        }


        /**
         * Teardown display of the current object
         **/
        public override void Nuke(OpenGL gl)
        {
            //tx.Destroy(gl);

            //s = null;
            //tx = null;
        }
    }
}
