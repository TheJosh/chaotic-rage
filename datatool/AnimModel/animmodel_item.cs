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
        private List<meshframe_item> meshframes;

        public animmodel_item()
        {
            meshframes = new List<meshframe_item>();
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
        public List<meshframe_item> Meshframes
        {
            get { return this.meshframes; }
        }
    }


    public class meshframe_item : base_item
    {
        private int frame;
        private string mesh;
        private string texture;


        public meshframe_item()
        {
        }

        override public string getName()
        {
            return this.mesh;
        }

        [Category("Data")]
        [ConfuseString("frame")]
        public int Frame
        {
            get { return this.frame; }
            set { this.frame = value; }
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
