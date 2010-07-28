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
            load_particletypes();


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


        /**
         * Loads the particletypes
         **/
        private void load_particletypes()
        {
            ConfuseSection sect = null;
            string file = System.IO.File.ReadAllText(this.datapath + "\\particletypes\\particletypes.conf");

            // Parse config
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading particletypes:\n" + ex.Message);
                Application.Exit();
            }

            // Load areatypes
            foreach (ConfuseSection s in sect.subsections) {
                particletype_item i = new particletype_item("");
                particletypes.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Particletype defined without a name");
                if (!s.values.ContainsKey("image")) throw new Exception("Particletype defined without an image");

                i.Name = s.get_string("name", "");
                i.Image = s.get_string("image", "");
                i.NumFrames = s.get_int("num_frames", 1);
                i.LinSpeed = s.get_range("lin_speed", new range(0)).toString();
                i.LinAccel = s.get_range("lin_accel", new range(0)).toString();
                i.Age = s.get_range("age", new range(0)).toString();

                load_actions(i, s);
            }
        }


        /**
         * Loads the actions for something
         * The specailised classes do not get loaded at this time - they will be swapped in later
         **/
        private void load_actions(actions_item i, ConfuseSection sect)
        {
            foreach (ConfuseSection s in sect.subsections) {
                if (s.name != "action") continue;

                int event_id = s.get_int("event", 0);
                int type_id = s.get_int("type", 0);

                action a = new action();
                a.Event = (action_event) event_id;
                a.Type = (action_type) type_id;
                
                if (s.values.ContainsKey("args")) {
                    a.loadinfo = s.values["args"];
                }

                i.addAction (a);
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
