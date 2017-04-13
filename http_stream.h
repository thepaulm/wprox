#ifndef _HTTP_STREAM_H
#define _HTTP_STREAM_H

enum http_type {
	NONE, GET, POST, PUT, DELETE, UNKNOWN
};

class http_stream
{
public:
	http_stream();
	~http_stream();

	void reset(void);
	void input(const char *buf, int len);
	int is_stream_end(void);

private:
	int m_bInHdr;
	int m_ContentLength;
	int m_ContentRead;
	enum http_type m_type;
};


#endif /* _HTTP_STREAM_H */
