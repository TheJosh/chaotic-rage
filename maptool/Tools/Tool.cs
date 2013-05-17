using System;
using System.Collections.Generic;
using System.Text;
using datatool;

namespace Maptool
{
    abstract class Tool
    {
        public abstract Entity createEntity();

        public abstract bool isThisOurs(Entity ent);

        public abstract bool isAreaBased();

        public abstract List<EntityType> getTypes();

        public abstract string saveConfig(Entity ent);

        public abstract Entity loadConfig(ConfuseSection sect);
    }
}
