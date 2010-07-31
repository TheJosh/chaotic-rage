using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    public class particletype_item : actions_item
    {
        private string name;
        private string image;
        private bool directional;
        private int num_frames;
        private range lin_speed;
        private range lin_accel;
        private range age;
        private range damage;
        private range damage_accel;


        public particletype_item(string name)
        {
            this.name = name;

            this.directional = false;
            this.num_frames = 1;

            this.lin_speed = new range(1);
            this.lin_accel = new range(1);
            this.age = new range(1);
            this.damage = new range(0);
            this.damage_accel = new range(0);
        }

        override public string getName()
        {
            return this.name;
        }



        [DescriptionAttribute("The name of this particletype, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The name of the image to use for this particletype")]
        [Editor(typeof(image_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string Image
        {
            get { return this.image; }
            set { this.image = value; }
        }

        [DescriptionAttribute("If set to true, the sprite chosen is influenced by the angle of movement")]
        public Boolean Directional
        {
            get { return this.directional; }
            set { this.directional = value; }
        }

        [DescriptionAttribute("The number of frames in the animation")]
        public int NumFrames
        {
            get { return this.num_frames; }
            set { this.num_frames = value; }
        }

        [DescriptionAttribute("Initial speed")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string LinSpeed
        {
            get { return this.lin_speed.toString(); }
            set { this.lin_speed = range.fromString(value); }
        }

        [DescriptionAttribute("Acceleration of the speed, in pixels/second")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string LinAccel
        {
            get { return this.lin_accel.toString(); }
            set { this.lin_accel = range.fromString(value); }
        }

        [DescriptionAttribute("The max age of the particle")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string Age
        {
            get { return this.age.toString(); }
            set { this.age = range.fromString(value); }
        }

        [DescriptionAttribute("Initial amount of damage the particle causes upon impact")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string Damage
        {
            get { return this.damage.toString(); }
            set { this.damage = range.fromString(value); }
        }

        [DescriptionAttribute("Acceleration of the damage amount, in hp/second")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string DamageAccel
        {
            get { return this.damage_accel.toString(); }
            set { this.damage_accel = range.fromString(value); }
        }
    }
}
