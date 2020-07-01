import -monochrome -define png:compression-strategy=0 -define png:compression-level=0 -define png:compression-filter=0 png:imp0.png
import -monochrome -define png:compression-strategy=0 -define png:compression-level=0 -define png:compression-filter=0 png32:imp32.png
import -monochrome -negate -depth 1 -compress none pbm:imp0.pbm
import -monochrome -negate -depth 1 pbm:imp1.pbm
import -monochrome -negate -depth 1 gray:imp1.raw
import -monochrome -negate -depth 8 gray:imp8.raw
import -monochrome -negate -depth 8 rgba:imp32.raw
import -monochrome -negate -depth 1 mono:imp0.mono
import -monochrome -negate -depth 1 xpm:imp1.xpm
import -monochrome -negate -depth 8 xpm:imp8.xpm
