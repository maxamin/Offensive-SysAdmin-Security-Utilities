namespace c2dotnet
{
    public class ServerApi: BaseClass
    {
        private List<ServerApiHandler> handlers = new List<ServerApiHandler>();
        public delegate Task ServerApiCallback(HttpContext context);
        public struct ServerApiHandler
        {
            public string Name;
            public ServerApiCallback Callback;
        }

        public ServerApi(C2Configuration c2configuration) : base(c2configuration)
        {
        }

        public void Handler(IApplicationBuilder app)
        {
            app.Run(async context =>
            {
                string apiCall = "error";
                try
                {
                    // /url/api/method apiCall = method
                    string[] uri = context.Request.Path.Value.Split("/");
                    apiCall = uri.Last();
                } catch(Exception)
                {

                }

                // Check if HandlerExists
                ServerApiHandler sah;
                if(IsHandlerDefined(apiCall))
                {
                    sah = GetHandler(apiCall);
                } else
                {
                    sah = GetHandler("error");
                }

                await sah.Callback(context);

            });
        }

        private void RegisterHandlers() {
            AddHandler("error", ErrorHandler);
        }

        private bool IsHandlerDefined(string name)
        {
            if(handlers.Any(e => e.Name == name))
            {
                return true;
            }
            return false;
        }

        private ServerApiHandler GetHandler(string name)
        {
            return handlers.Find(e => e.Name == name);
        }
        private void AddHandler(string name, ServerApiCallback callback)
        {
            ServerApiHandler sah = new ServerApiHandler();
            sah.Name = name;
            sah.Callback = callback;
            handlers.Add(sah);
        }

        private async Task ErrorHandler(HttpContext context)
        {
            ResponseBuilder(context);
        }

        private async Task ResponseBuilder(HttpContext context, int code = 200)
        {
            Send(context, (object)null, code);
        }

        private async Task Send(HttpContext context, object o, int code = 200)
        {
            context.Response.StatusCode = code;
            await context.Response.WriteAsync("working");
        }

    }
}
