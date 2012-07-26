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

        private int angle_range;
        private int fire_delay;
        private int reload_delay;
        private bool continuous;
        private int magazine_limit;
        private int belt_limit;
        private float range;
        private float unit_damage;
        private float wall_damage;

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
            b.values["angle_range"] = this.angle_range;
            b.values["fire_delay"] = this.fire_delay;
            b.values["reload_delay"] = this.reload_delay;
            b.values["continuous"] = this.continuous;
            b.values["magazine_limit"] = this.magazine_limit;
            b.values["belt_limit"] = this.belt_limit;
            b.values["range"] = this.range;
            b.values["wall_damage"] = this.wall_damage;
            b.values["unit_damage"] = this.unit_damage;

            string o = b.ToString();

            foreach (weapontype_sound i in this.sounds)
            {
                if (i.Type != 0) o += "\n\tsound { " + i.getConfItem() + "}";
            }

            return o;
        }


        /**
         * Returns the list of metrics available
         **/
        override public List<string> getMetricNames()
        {
            List<string> o = new List<string>();
            o.Add("Num fire in 10 secs");
            o.Add("Num fire in 30 secs");
            o.Add("Unit Damage in 10 secs");
            o.Add("Unit Damage in 30 secs");
            o.Add("Unit Damage in 10 secs (angle adjusted)");
            o.Add("Unit Damage in 30 secs (angle adjusted)");
            return o;
        }

        /**
         * Returns the data for the various metrics
         **/
        override public float getMetric(int type)
        {
            int time = 0;
            int metric = 0;

            switch (type) {
                case 0: metric = 1; time = 10 * 1000; break;
                case 1: metric = 1; time = 30 * 1000; break;

                case 2: metric = 2; time = 10 * 1000; break;
                case 3: metric = 2; time = 30 * 1000; break;

                case 4: metric = 3; time = 10 * 1000; break;
                case 5: metric = 3; time = 30 * 1000; break;
            }

            if (time == 0) return 0;
            if (metric == 0) return 0;


            // The weights are to make the assumption
            // that the player is partway into a clip, and partway into
            // a reload cycle when the imaginary '10 secs' begins
            //
            int i = (int)Math.Round(this.fire_delay * 0.6);
            int num_mag = (int)Math.Round(this.magazine_limit * 0.75);
            int num_fire = 0;

            while (i <= time) {
                num_fire++;
                num_mag--;

                if (num_mag == 0) {
                    num_mag = this.magazine_limit;
                    if (num_fire >= this.belt_limit) break;
                    i += this.reload_delay;
                }
                
                i += this.fire_delay;
            }

            if (metric == 1) {
                return num_fire;
            } else if (metric == 2) {
                return num_fire * this.unit_damage;
            } else if (metric == 3) {
                float angweight = 1.0f / this.angle_range * 5.0f;
                return num_fire * this.unit_damage * angweight;
            }

            return 0;
        }



        [DescriptionAttribute("The name of this weapon, internal use only")]
        [Category("Metadata")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The name of this weapon in-game")]
        [Category("Metadata")]
        public string Title
        {
            get { return this.title; }
            set { this.title = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public int AngleRange
        {
            get { return this.angle_range; }
            set { this.angle_range = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public int FireDelay
        {
            get { return this.fire_delay; }
            set { this.fire_delay = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public int ReloadDelay
        {
            get { return this.reload_delay; }
            set { this.reload_delay = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public bool Continuous
        {
            get { return this.continuous; }
            set { this.continuous = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public int MagazineLimit
        {
            get { return this.magazine_limit; }
            set { this.magazine_limit = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public int BeltLimit
        {
            get { return this.belt_limit; }
            set { this.belt_limit = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public float Range
        {
            get { return this.range; }
            set { this.range = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public float UnitDamage
        {
            get { return this.unit_damage; }
            set { this.unit_damage = value; }
        }

        [DescriptionAttribute("no desc yet")]
        [Category("Specifications")]
        public float WallDamage
        {
            get { return this.wall_damage; }
            set { this.wall_damage = value; }
        }

        [Browsable(false)]
        public List<weapontype_sound> Sounds
        {
            get { return this.sounds; }
        }

    }
}
