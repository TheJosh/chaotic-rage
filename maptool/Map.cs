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
        public Bitmap Terrain;
        public Bitmap Heightmap;
        public int Width;
        public int Height;

        public List<Entity> Entities
        {
            get { return entities; }
        }


        public Map()
        {
            this.entities = new List<Entity>();
            this.Terrain = new Bitmap(200, 200);
            this.Heightmap = new Bitmap(200, 200);
            this.Width = 200;
            this.Height = 200;
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

            this.Width = parsed.get_int("width", 200);
            this.Height = parsed.get_int("height", 200);

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
            this.Terrain = new Bitmap(basename + "\\terrain.png");
            this.Heightmap = new Bitmap(basename + "\\heightmap.png");

            return true;
        }
    }
}
