using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    public class particlegenerator_spew
    {
        private particletype_item type;
        private int angle_range;
        private int rate;
        private int time;


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

        [DescriptionAttribute("The amount of time to release particles for")]
        public int Time
        {
            get { return this.time; }
            set { this.time = value; }
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

        [DescriptionAttribute("The available spewers")]
        public List<particlegenerator_spew> Spewers
        {
            get { return this.spewers; }
        }

    }
}
