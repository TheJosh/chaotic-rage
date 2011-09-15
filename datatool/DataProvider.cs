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

            // Load modifiers
            modifiers.Add(new modifier_item("Shield"));
            modifiers.Add(new modifier_item("Speed"));

            // Load particle generators
            particlegenerators.Add(new particlegenerator_item("Machine gun"));
            particlegenerators.Add(new particlegenerator_item("Blood"));

            // Load unit classes
            unitclasses.Add(new unitclass_item("Maniac"));
            unitclasses.Add(new unitclass_item("Zombie"));

            // Load unit classes
            weapontypes.Add(new weapontype_item("Shotgun"));
            weapontypes.Add(new weapontype_item("Machine gun"));

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

    }
}
