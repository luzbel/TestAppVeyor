extern "C" void createPlugin(const char *name,void**a);

int main()
{
	void **a;
	createPlugin("crow",a);
	return 0;
}
