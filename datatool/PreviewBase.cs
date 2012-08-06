using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SharpGL;

namespace datatool
{
    public abstract class PreviewBase
    {
        /**
         * Init display of a given object
         **/
        public virtual void Init(OpenGL gl, base_item item)
        {
        }

        /**
         * Render
         **/
        abstract public void Render(OpenGL gl);

        /**
        * Teardown display of the current object
        **/
        public virtual void Nuke(OpenGL gl)
        {
        }
    }
}
