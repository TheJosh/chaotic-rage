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
        private List<base_item> particletypes;
        private List<base_item> particlegenerators;
        private List<base_item> unitclasses;


        public bool load(string filename)
        {
            this.datapath = filename;
            this.read = new ConfuseReader();

            this.areatypes = new List<base_item>();
            this.particletypes = new List<base_item>();
            this.particlegenerators = new List<base_item>();
            this.unitclasses = new List<base_item>();

            load_areatypes();

            // Load particle types
            
            particletypes.Add(new particletype_item("Machine gun"));
            particletypes.Add(new particletype_item("Blood"));

            // Load particle generators
            
            particlegenerators.Add(new particlegenerator_item("Machine gun"));
            particlegenerators.Add(new particlegenerator_item("Blood"));

            // Load unit classes
            
            unitclasses.Add(new unitclass_item("Maniac"));
            unitclasses.Add(new unitclass_item("Zombie"));

            

            return true;
        }

        /**
         * Loads the areatypes
         **/
        private void load_areatypes() {
            ConfuseSection sect = null;
            string file = System.IO.File.ReadAllText(this.datapath + "\\areatypes\\areatypes.conf");
            
            // Parse config
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading areatypes:\n" + ex.Message);
                Application.Exit();
            }

            // Load areatypes
            foreach (ConfuseSection s in sect.subsections) {
                areatype_item i = new areatype_item("");
                areatypes.Add(i);

                if (! s.values.ContainsKey("name")) throw new Exception("Areatype defined without a name");
                if (!s.values.ContainsKey("image")) throw new Exception("Areatype defined without an image");

                i.Name = s.get_string("name", "");
                i.Image = s.get_string("image", "");
                i.Stretch = s.get_bool("stretch", false);
                i.Wall = s.get_bool("wall", false);
            }
        }

        public List<base_item> AreaTypes
        {
            get { return areatypes; }
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

    }
}
