using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace Maptool
{
    class ZoneEntity : Entity
    {
        private int width;
        private int height;
        private ZoneFactions spawn;
        private ZoneFactions prison;
        private ZoneFactions collect;
        private ZoneFactions dest;
        private ZoneFactions nearbase;


        public ZoneEntity()
        {
            this.width = 20;
            this.height = 20;
            this.spawn = new ZoneFactions();
            this.prison = new ZoneFactions();
            this.collect = new ZoneFactions();
            this.dest = new ZoneFactions();
            this.nearbase = new ZoneFactions();
        }

        [Category("Size")]
        [Description("The width of this zone")]
        public int Width
        {
            get { return width; }
            set { width = value; }
        }

        [Category("Size")]
        [Description("The height of this zone")]
        public int Height
        {
            get { return height; }
            set { height = value; }
        }

        [Category("Purpose")]
        [Description("Spawn point for units")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Spawn
        {
            get { return spawn; }
            set { spawn = value; }
        }

        [Category("Purpose")]
        [Description("Prison that captured units get placed into")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Prison
        {
            get { return prison; }
            set { prison = value; }
        }

        [Category("Purpose")]
        [Description("Items which need to be picked up (i.e. flags)")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Collection
        {
            get { return collect; }
            set { collect = value; }
        }

        [Category("Purpose")]
        [Description("Delivery location of picked-up items")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Destination
        {
            get { return dest; }
            set { dest = value; }
        }

        [Category("Purpose")]
        [Description("A general-purpose location near the base but not in it")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions NearBase
        {
            get { return nearbase; }
            set { nearbase = value; }
        }
    }
}
