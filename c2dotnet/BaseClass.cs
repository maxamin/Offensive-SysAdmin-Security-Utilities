namespace c2dotnet
{
    public class BaseClass
    {
        protected C2Configuration c2configuration;

        public BaseClass(C2Configuration c2configuration)
        {
            this.c2configuration = c2configuration;
        }

        public string Me()
        {
            return this.GetType().Name;
        }
    }
}
