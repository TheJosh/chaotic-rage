using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public enum action_event {
        UNDEFINED = 0,
        HIT_WALL = 1,
        HIT_UNIT = 2,
        HIT_PARTICLE = 3,
    }

    public enum action_type
    {
        UNDEFINED = 0,
        ADD_PGENERATOR = 1,
    }


    /**
     * An action
     **/
    [Serializable]
    public class action
    {
        protected action_event e;
        protected action_type type;

        [Category("General")]
        [Description("The event to fire this action on")]
        public action_event Event
        {
            get { return e; }
            set { e = value; }
        }

        [Category("General")]
        [Description("The type of action to call")]
        public action_type Type
        {
            get { return type; }
            set { type = value; }
        }
    }


    [Serializable]
    public class action_pgenerator : action
    {
        public action_pgenerator(action_event e)
        {
            this.e = e;
            this.type = action_type.ADD_PGENERATOR;
        }

        private particlegenerator_item generator;

        [Category("Parameters")]
        [Description("The particle generator to spawn")]
        [Editor(typeof(particlegenerator_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public particlegenerator_item Generator
        {
            get { return generator; }
            set { generator = value; }
        }
    }
}
