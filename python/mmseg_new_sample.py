# -*- coding: utf-8 -*-

#现在是新的接口。
import mmseg

manager = mmseg.SegmenterManager()
manager.Init('/usr/local/data/mmseg/', 1)
segmenter = manager.GetSegmenter(False)
buff = (u'中文分词艺术电影，中国，爱迪生，爱因斯坦').encode('utf-8')
segmenter.SetBuffer(buff, len(buff))
token = mmseg.SegmentedToken()
    
while segmenter.GetNextToken(token, 0):
    print 'begin:', token.begin()
    print 'end:', token.end()
    print 'word:', token.word()
    print 'token_type:', token.token_type()
    
print 'done!'