using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Text;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MakeSpriteFont
{
    public class PrivateFontCollectionImporter : TrueTypeImporter
    {
        // Attempts to instantiate the requested GDI+ font object.
        
        private FontFamily _fontFamily;

        protected override Font CreateFont(CommandLineOptions options)
        {
            PrivateFontCollection fontCollection = new PrivateFontCollection();
            fontCollection.AddFontFile(options.SourceFont);
            _fontFamily = fontCollection.Families.FirstOrDefault();
            if (_fontFamily == null)
            {
                throw new Exception(string.Format("Unable to load font from '{0}'.", options.SourceFont));
            }

            Font font = new Font(_fontFamily, PointsToPixels(options.FontSize), options.FontStyle, GraphicsUnit.Pixel);

            //string fontName = Path.GetFileNameWithoutExtension(options.SourceFont);
            string fontName = _fontFamily.Name;

            try
            {
                // The font constructor automatically substitutes fonts if it can't find the one requested.
                // But we prefer the caller to know if anything is wrong with their data. A simple string compare
                // isn't sufficient because some fonts (eg. MS Mincho) change names depending on the locale.

                // Early out: in most cases the name will match the current or invariant culture.
                if (fontName.Equals(font.FontFamily.GetName(CultureInfo.CurrentCulture.LCID), StringComparison.OrdinalIgnoreCase) ||
                    fontName.Equals(font.FontFamily.GetName(CultureInfo.InvariantCulture.LCID), StringComparison.OrdinalIgnoreCase))
                {
                    return font;
                }

                // Check the font name in every culture.
                foreach (CultureInfo culture in CultureInfo.GetCultures(CultureTypes.SpecificCultures))
                {
                    if (fontName.Equals(font.FontFamily.GetName(culture.LCID), StringComparison.OrdinalIgnoreCase))
                    {
                        return font;
                    }
                }

                // A font substitution must have occurred.
                throw new Exception(string.Format("Can't find font '{0}'.", options.SourceFont));
            }
            catch
            {
                font.Dispose();
                throw;
            }
        }
    }
}
