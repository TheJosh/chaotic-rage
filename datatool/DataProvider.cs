using System;
using System.Collections.Generic;
using System.Text;

namespace datatool
{
    public class DataProvider
    {
        private List<base_item> areatypes;
        private List<base_item> particletypes;
        private List<base_item> particlegenerators;
        private List<base_item> unitclasses;

        public string datapath;

        public bool load(string filename)
        {
            // Load area types
            areatypes = new List<base_item>();
            areatypes.Add(new areatype_item("Dirt"));
            areatypes.Add(new areatype_item("Brick"));

            // Load particle types
            particletypes = new List<base_item>();
            particletypes.Add(new particletype_item("Machine gun"));
            particletypes.Add(new particletype_item("Blood"));

            // Load particle generators
            particlegenerators = new List<base_item>();
            particlegenerators.Add(new particlegenerator_item("Machine gun"));
            particlegenerators.Add(new particlegenerator_item("Blood"));

            // Load unit classes
            unitclasses = new List<base_item>();
            unitclasses.Add(new unitclass_item("Maniac"));
            unitclasses.Add(new unitclass_item("Zombie"));

            this.datapath = filename;

            return true;
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
