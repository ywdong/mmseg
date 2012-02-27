# -*- coding: utf-8 -*-

import cmmseg
#cmmseg.init('F:\\deps\\mmseg\\src\\win32')
seg = cmmseg.MMSeg('/usr/local/data/mmseg')
rs = seg.segment((u'中国人民银行中文分词，爱因斯坦').encode('utf-8'))
for token in rs:
    print token['word']
    print token['begin']
    print token['end']
    print token['token_type']