using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    public class particlegenerator_spew : base_item
    {
        private string name;
        private particletype_item type;
        private int angle_range;
        private int rate;
        private int time;
        private int delay;
        private int offset;

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

        [DescriptionAttribute("The particle to spew")]
        [Editor(typeof(particletype_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public particletype_item Type
        {
            get { return this.type; }
            set { this.type = value; }
        }

        [DescriptionAttribute("The available angle to release particles within")]
        public int AngleRange
        {
            get { return this.angle_range; }
            set { this.angle_range = value; }
        }

        [DescriptionAttribute("The rate particles are released")]
        public int Rate
        {
            get { return this.rate; }
            set { this.rate = value; }
        }

        [DescriptionAttribute("A delay between releases, in milliseconds")]
        public int Delay
        {
            get { return this.delay; }
            set { this.delay = value; }
        }

        [DescriptionAttribute("The total amount of time to release particles for")]
        public int Time
        {
            get { return this.time; }
            set { this.time = value; }
        }

        [DescriptionAttribute("The distance, in pixels, to offset the created particles from the generator")]
        public int Offset
        {
            get { return this.offset; }
            set { this.offset = value; }
        }

    }

    [Serializable]
    public class particlegenerator_item : base_item
    {
        private string name;
        private List<particlegenerator_spew> spewers;


        public particlegenerator_item(string name)
        {
            this.name = name;
            this.spewers = new List<particlegenerator_spew>();

            this.spewers.Add(new particlegenerator_spew());
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

        [Browsable(false)]
        public List<particlegenerator_spew> Spewers
        {
            get { return this.spewers; }
        }

    }
}
