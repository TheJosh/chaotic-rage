using System;
using System.Collections.Generic;
using System.Text;

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
}
