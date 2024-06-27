namespace c2dotnet
{
    public class ClientApi: BaseClass
    {
        private HttpContext context;
        public ClientApi(C2Configuration c2configuration): base(c2configuration)
        {
        }

        public async Task ProcessRequest(HttpContext context)
        {
            this.context = context;
            await context.Response.WriteAsync("working");
        }
    }
}
