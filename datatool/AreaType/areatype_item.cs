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
        private areatype_item ground_type;
        private List<areatype_sound> sounds;


        public areatype_item(string name)
        {
            this.name = name;
            sounds = new List<areatype_sound>();
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

        /**
         * Is it a wall?
         **/
        [DescriptionAttribute("For walls only; what to show when the wall is destroyed")]
        [Editor(typeof(areatype_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public areatype_item GroundType
        {
            get { return this.ground_type; }
            set { this.ground_type = value; }
        }


        /**
         * Sounds
         **/
        [Browsable(false)]
        public List<areatype_sound> Sounds
        {
            get { return this.sounds; }
            set { this.sounds = value; }
        }
    }


    public enum areatype_sound_type {
        WALK_OVER,
    }

    [Serializable]
    public class areatype_sound : base_item
    {
        private string name;
        private areatype_sound_type type;

        override public string getName()
        {
            return this.name;
        }


        [DescriptionAttribute("The name of this sound, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("What this sound is for")]
        public areatype_sound_type Type
        {
            get { return this.type; }
            set { this.type = value; }
        }
    }
}
