using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.HttpsPolicy;
using Microsoft.AspNetCore.Routing;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace c2dotnet
{
    public class Startup
    {
        public IConfiguration configuration { get; }
        public Startup(IConfiguration configuration)
        {
            this.configuration = configuration;
        }

        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            C2Configuration c2configuration = C2Configuration.ParseConfig("config.json");
            ServerApi serverApi = new ServerApi(c2configuration);
            ClientApi clientApi = new ClientApi(c2configuration);

            if(!env.IsDevelopment())
            {
                app.UseExceptionHandler("/Error");
                app.UseHsts();
            }

            app.UseStaticFiles(new StaticFileOptions
            {
                OnPrepareResponse = context =>
                {
                    context.Context.Response.Headers.Add("Server", c2configuration.ServerResponseServer);
                }
            });

            app.MapWhen(context => context.Request.Path.StartsWithSegments($"/{c2configuration.ServerApiPath}"), serverApi.Handler);

            app.Run(async context => await clientApi.ProcessRequest(context));
        }
    }
}
