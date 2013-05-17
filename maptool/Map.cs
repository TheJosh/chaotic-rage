using System;
using System.Collections.Generic;
using System.Text;
using datatool;
using System.Drawing;

namespace Maptool
{
    public class Map
    {
        private List<Entity> entities;
        private Bitmap terrain;
        private Bitmap heightmap;

        public List<Entity> Entities
        {
            get { return entities; }
        }

        public Bitmap Terrain
        {
            get { return terrain; }
        }

        public Bitmap Heightmap
        {
            get { return heightmap; }
        }


        public Map()
        {
            this.entities = new List<Entity>();
            this.terrain = null;
            this.heightmap = null;
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

            this.entities = new List<Entity>();
            foreach (ConfuseSection sect in parsed.subsections) {
                Entity ent = null;

                if (sect.name == "zone") {
                    ent = Tools.Zone.loadConfig(sect);
                } else if (sect.name == "light") {
                    ent = Tools.Light.loadConfig(sect);
                } else if (sect.name == "object") {
                    ent = Tools.Object.loadConfig(sect);
                }

                if (ent == null) continue;
                if (ent.Type == null) continue;

                entities.Add(ent);
            }
            
            string basename = System.IO.Path.GetDirectoryName(filename);
            this.terrain = new Bitmap(basename + "\\terrain.png");
            this.heightmap = new Bitmap(basename + "\\heightmap.png");

            return true;
        }
    }
}
