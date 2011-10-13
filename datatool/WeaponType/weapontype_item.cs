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


        override public string getConfItem()
        {
            ConfuseBuilder b = new ConfuseBuilder();

            b.values["sound"] = this.sound;
            b.values["type"] = this.type;

            return b.ToString();
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
        private int fire_delay;
        private int reload_delay;
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


        override public string getConfItem()
        {
            ConfuseBuilder b = new ConfuseBuilder();

            b.values["name"] = this.name;
            b.values["title"] = this.title;
            b.values["particlegen"] = this.particlegen;
            b.values["particle"] = this.particle;
            b.values["angle_range"] = this.angle_range;
            b.values["fire_delay"] = this.fire_delay;
            b.values["reload_delay"] = this.reload_delay;
            b.values["continuous"] = this.continuous;
            b.values["magazine_limit"] = this.magazine_limit;
            b.values["belt_limit"] = this.belt_limit;

            string o = b.ToString();

            foreach (weapontype_sound i in this.sounds)
            {
                if (i.Type != 0) o += "\n\tsound { " + i.getConfItem() + "}";
            }

            return o;
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
        public int FireDelay
        {
            get { return this.fire_delay; }
            set { this.fire_delay = value; }
        }

        [DescriptionAttribute("no desc yet")]
        public int ReloadDelay
        {
            get { return this.reload_delay; }
            set { this.reload_delay = value; }
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
