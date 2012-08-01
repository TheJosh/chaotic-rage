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
                        p.SetValue(this, sect.get_int(((ConfuseInt) att).Name, 0), null);
                    } else if (att is ConfuseFloat) {
                        p.SetValue(this, sect.get_float(((ConfuseFloat) att).Name, 0), null);
                    } else if (att is ConfuseString) {
                        p.SetValue(this, sect.get_string(((ConfuseString)att).Name, ""), null);
                    } else if (att is ConfuseBoolean) {
                        p.SetValue(this, sect.get_bool(((ConfuseBoolean)att).Name, false), null);
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
        public ConfuseInt(string name) : base(name) { }
    }

    public class ConfuseFloat : ConfuseBinding {
        public ConfuseFloat(string name) : base(name) { }
    }

    public class ConfuseString : ConfuseBinding {
        public ConfuseString(string name) : base(name) { }
    }

    public class ConfuseBoolean : ConfuseBinding {
        public ConfuseBoolean(string name) : base(name) { }
    }

}
