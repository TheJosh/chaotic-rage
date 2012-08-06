using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public class animmodel_item : base_item 
    {
        private string name;
        private string mesh;
        private string texture;


        public animmodel_item()
        {
        }

        override public string getName()
        {
            return this.name;
        }


        [Category("General")]
        [ConfuseString("name")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [Category("Data")]
        [ConfuseString("mesh")]
        public string Mesh
        {
            get { return this.mesh; }
            set { this.mesh = value; }
        }

        [Category("Data")]
        [ConfuseString("texture")]
        public string Texture
        {
            get { return this.texture; }
            set { this.texture = value; }
        }

    }
}
