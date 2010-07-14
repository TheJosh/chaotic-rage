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
     * Base items which have actions
     **/
    [Serializable]
    public abstract class actions_item : base_item
    {
        private List<action> actions;

        public actions_item()
        {
            actions = new List<action>();
        }

        public void addAction(action a)
        {
            actions.Add(a);
        }

        public void replaceAction(action old, action nu)
        {
            int index = actions.FindIndex(action => action == old);
            if (index == -1) return;
            actions[index] = nu;
        }

        public void removeAction(action a)
        {
            actions.Remove(a);
        }

        public List<action> getActions()
        {
            return actions;
        }
    }
}
