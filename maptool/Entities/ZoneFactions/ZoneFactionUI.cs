using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace Maptool
{
    public class ZoneFactionsUI : System.Drawing.Design.UITypeEditor
    {
        public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return System.Drawing.Design.UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            frmZoneFaction form = new frmZoneFaction();

            form.Value = (ZoneFactions)value;

            if (form.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                return form.Value;
            }

            return value;
        }
    }
}
