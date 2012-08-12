using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

namespace datatool
{
    /**
     * Regular base items
     **/
    [Serializable]
    public abstract class base_item
    {
        abstract public string getName();

        override public string ToString()
        {
            return this.getName();
        }

        virtual public string getConfItem()
        {
            return "";
        }

        virtual public List<string> getMetricNames()
        {
            return null;
        }

        virtual public float getMetric(int type)
        {
            return 0;
        }

        public void fillData(ConfuseSection sect)
        {
            PropertyInfo [] props = this.GetType().GetProperties();
            object [] attrs;

            foreach (PropertyInfo p in props) {
                attrs = p.GetCustomAttributes(typeof(ConfuseBinding), true);
                foreach (object att in attrs) {

                    if (att is ConfuseInt) {
                        p.SetValue(this, sect.get_int(((ConfuseInt)att).Name, ((ConfuseInt)att).Default), null);
                    } else if (att is ConfuseFloat) {
                        p.SetValue(this, sect.get_float(((ConfuseFloat)att).Name, ((ConfuseFloat)att).Default), null);
                    } else if (att is ConfuseString) {
                        p.SetValue(this, sect.get_string(((ConfuseString)att).Name, ((ConfuseString)att).Default), null);
                    } else if (att is ConfuseBoolean) {
                        p.SetValue(this, sect.get_bool(((ConfuseBoolean)att).Name, ((ConfuseBoolean)att).Default), null);
                    }

                }
            }
        }
    }


    /**
    * Adds a property to the list item
    **/
    class ExtraListItem : System.Windows.Forms.ListViewItem
    {
        private base_item extra;

        public ExtraListItem()
        {
            this.ImageIndex = -1;
        }

        public ExtraListItem(base_item item)
        {
            this.ImageIndex = -1;
            this.Extra = item;
        }


        public base_item Extra
        {
            get { return this.extra; }
            set
            {
                this.extra = value;
                if (value.getName() != null)
                {
                    this.Text = value.getName();
                }
                else
                {
                    this.Text = "- Nothing -";
                }
            }
        }
    }


    abstract public class ConfuseBinding : Attribute
    {
        private string name;
        public ConfuseBinding(string name)
            : base()
        {
            this.name = name;
        }
        public string Name { get { return name; } }
    }

    public class ConfuseInt : ConfuseBinding {
        private int def;
        public ConfuseInt(string name, int def = 0) : base(name) { this.def = def; }
        public int Default { get { return def; } }
    }

    public class ConfuseFloat : ConfuseBinding {
        private float def;
        public ConfuseFloat(string name, float def = 0) : base(name) { }
        public float Default { get { return def; } }
    }

    public class ConfuseString : ConfuseBinding {
        private string def;
        public ConfuseString(string name, string def = "") : base(name) { }
        public string Default { get { return def; } }
    }

    public class ConfuseBoolean : ConfuseBinding {
        private bool def;
        public ConfuseBoolean(string name, bool def = false) : base(name) { }
        public bool Default { get { return def; } }
    }

}
