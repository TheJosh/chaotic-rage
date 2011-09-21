using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public class DataProvider
    {
        ConfuseReader read;
        public string datapath;

        private List<base_item> areatypes;
        private List<base_item> modifiers;
        private List<base_item> particletypes;
        private List<base_item> particlegenerators;
        private List<base_item> unitclasses;
        private List<base_item> weapontypes;


        /**
        * Loads data files
        **/
        public bool load(string filename)
        {
            this.datapath = filename;
            this.read = new ConfuseReader();

            this.areatypes = new List<base_item>();
            this.modifiers = new List<base_item>();
            this.particletypes = new List<base_item>();
            this.particlegenerators = new List<base_item>();
            this.unitclasses = new List<base_item>();
            this.weapontypes = new List<base_item>();

            load_particletypes();
            load_weapontypes();

            return true;
        }


        /**
        * Saves data files
        **/
        public bool save(string filename)
        {
            string o;

            // Particles 
            o = "";
            foreach (base_item i in this.particletypes)
            {
                o += "particle {\n\t" + i.getConfItem() + "\n}\n";
            }
            System.IO.File.WriteAllText(this.datapath + "\\particletypes.conf", o);

            // Weapons
            o = "";
            foreach (base_item i in this.weapontypes) {
                o += "weapon {\n\t" + i.getConfItem() + "\n}\n";
            }
            System.IO.File.WriteAllText(this.datapath + "\\weapontypes.conf", o);

            return true;
        }


        /**
         * Loads the particletypes
         **/
        private void load_particletypes()
        {
            ConfuseSection sect = null;
            string file = System.IO.File.ReadAllText(this.datapath + "\\particletypes.conf");

            // Parse config
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading particletypes:\n" + ex.Message);
                Application.Exit();
            }

            // Load particletypes
            foreach (ConfuseSection s in sect.subsections) {
                particletype_item i = new particletype_item("");
                particletypes.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Particletype defined without a name");

                i.Name = s.get_string("name", "");
                i.Model = s.get_string("model", "");
                i.MaxSpeed = s.get_range("max_speed", new range(0)).toString();
                i.BeginSpeed = s.get_range("begin_speed", new range(0)).toString();
                i.Accel = s.get_range("accel", new range(0)).toString();
                i.Age = s.get_range("age", new range(0)).toString();
                i.UnitDamage = s.get_range("unit_damage", new range(0)).toString();
                i.WallDamage = s.get_range("wall_damage", new range(0)).toString();
                i.UnitHits = s.get_range("unit_hits", new range(0)).toString();
                i.WallHits = s.get_range("wall_hits", new range(0)).toString();
            }
        }


        /**
        * Loads the particletypes
        **/
        private void load_weapontypes()
        {
            ConfuseSection sect = null;
            string file = System.IO.File.ReadAllText(this.datapath + "\\weapontypes.conf");

            // Parse config
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading weapontypes:\n" + ex.Message);
                Application.Exit();
            }

            // Load particletypes
            foreach (ConfuseSection s in sect.subsections) {
                weapontype_item i = new weapontype_item("");
                weapontypes.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Weapontype defined without a name");

                i.Name = s.get_string("name", "");
                i.Title = s.get_string("title", "");

                i.Particle = this.FindParticletype(s.get_string("particle", ""));
                i.AngleRange = s.get_int("angle_range", 0);
                i.Rate = s.get_int("rate", 0);
                i.Continuous = s.get_bool("continuous", false);
                i.MagazineLimit = s.get_int("magazine_limit", 100);
                i.BeltLimit = s.get_int("belt_limit", 1000);

                i.ParticleGen = this.FindParticlegenerator(s.get_string("particlegen", ""));
            }
        }


        public List<base_item> AreaTypes
        {
            get { return areatypes; }
        }

        public List<base_item> Modifiers
        {
            get { return modifiers; }
        }

        public List<base_item> ParticleTypes
        {
            get { return particletypes; }
        }

        public List<base_item> ParticleGenerators
        {
            get { return particlegenerators; }
        }

        public List<base_item> UnitClasses
        {
            get { return unitclasses; }
        }

        public List<base_item> WeaponTypes
        {
            get { return weapontypes; }
        }



        public particletype_item FindParticletype(string name)
        {
            if (name == null || name == "") return null;
            foreach (particletype_item item in particletypes) {
                if (item.Name == name) return item;
            }
            return null;
        }

        public particlegenerator_item FindParticlegenerator(string name)
        {
            if (name == null || name == "") return null;
            foreach (particlegenerator_item item in particlegenerators) {
                if (item.Name == name) return item;
            }
            return null;
        }

    }
}
