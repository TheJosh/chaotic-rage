using System;
using System.Collections.Generic;
using System.Text;
using datatool;

namespace Maptool
{
    class Map
    {
        private List<Entity> entities;

        public List<Entity> Entities
        {
            get { return entities; }
            set { entities = value; }
        }


        /**
         * Saves a map
         **/
        public bool Save(string filename)
        {
            string o = "";

            foreach (Entity ent in entities) {
                if (ent is ZoneEntity) {
                    o += Tools.Zone.saveConfig(ent) + "\n";
                }
            }

            System.IO.File.WriteAllText(filename, o);

            return true;
        }


        /**
         * Load
         **/
        public bool Load(string filename)
        {
            ConfuseReader rdr = new ConfuseReader();
            string input = System.IO.File.ReadAllText(filename);
            ConfuseSection parsed = rdr.Parse(input);

            entities = new List<Entity>();

            foreach (ConfuseSection sect in parsed.subsections) {
                Entity ent = null;

                if (sect.name == "zone") {
                    ent = Tools.Zone.loadConfig(sect);
                }

                if (ent == null) continue;
                if (ent.Type == null) continue;

                entities.Add(ent);
            }
            
            return true;
        }
    }
}
