using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{

    [Serializable]
    public class areatype_item : base_item
    {
        private string name;
        private string image;
        private bool stretch;
        private bool wall;


        public areatype_item(string name)
        {
            this.name = name;
        }

        override public string getName()
        {
            return this.name;
        }


        /**
         * The name
         **/
        [DescriptionAttribute("The name of this areatype, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        /**
         * The image to show
         **/
        [DescriptionAttribute("The name of the image to use for this areatype")]
        [Editor(typeof(image_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string Image
        {
            get { return this.image; }
            set { this.image = value; }
        }

        /**
         * Stretch the image instead of tiling it
         **/
        [DescriptionAttribute("If set to true, the image will be stretched, otherwise it will be tiled")]
        public Boolean Stretch
        {
            get { return this.stretch; }
            set { this.stretch = value; }
        }

        /**
         * Is it a wall?
         **/
        [DescriptionAttribute("If set to true, the areatype will be a wall")]
        public Boolean Wall
        {
            get { return this.wall; }
            set { this.wall = value; }
        }
    }
}
