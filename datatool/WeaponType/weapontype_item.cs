using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    public class weapontype_sound : base_item
    {
        private string sound;
        private int type;


        override public string getName()
        {
            return this.sound;
        }


        [DescriptionAttribute("Sound")]
        public string Sound
        {
            get { return this.sound; }
            set { this.sound = value; }
        }

        [DescriptionAttribute("Type")]
        public int Type
        {
            get { return this.type; }
            set { this.type = value; }
        }


    }


    [Serializable]
    public class weapontype_item : base_item 
    {
        private string name;
        private string title;

        private particlegenerator_item particlegen;

        private particletype_item particle;
        private int angle_range;
        private int rate;
        private bool continuous;
        private int magazine_limit;
        private int belt_limit;

        private List<weapontype_sound> sounds;


        public weapontype_item(string name)
        {
            this.name = name;

            this.sounds = new List<weapontype_sound>();
            this.sounds.Add(new weapontype_sound());
        }

        override public string getName()
        {
            return this.name;
        }



        [DescriptionAttribute("The name of this weapon, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The name of this weapon in-game")] 
        public string Title
        {
            get { return this.title; }
            set { this.title = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Editor(typeof(particletype_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public particletype_item Particle
        {
            get { return this.particle; }
            set { this.particle = value; }
        }

        [DescriptionAttribute("no desc yet")]
        public int AngleRange
        {
            get { return this.angle_range; }
            set { this.angle_range = value; }
        }

        [DescriptionAttribute("no desc yet")]
        public int Rate
        {
            get { return this.rate; }
            set { this.rate = value; }
        }

        [DescriptionAttribute("no desc yet")]
        public bool Continuous
        {
            get { return this.continuous; }
            set { this.continuous = value; }
        }

        [DescriptionAttribute("no desc yet")]
        public int MagazineLimit
        {
            get { return this.magazine_limit; }
            set { this.magazine_limit = value; }
        }

        [DescriptionAttribute("no desc yet")]
        public int BeltLimit
        {
            get { return this.belt_limit; }
            set { this.belt_limit = value; }
        }


        [DescriptionAttribute("For effects only")]
        [Editor(typeof(particlegenerator_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public particlegenerator_item ParticleGen
        {
            get { return this.particlegen; }
            set { this.particlegen = value; }
        }


        [Browsable(false)]
        public List<weapontype_sound> Sounds
        {
            get { return this.sounds; }
        }

    }
}
