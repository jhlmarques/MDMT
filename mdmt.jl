using JuMP
using GLPK
using Logging
using ArgParse
using Dates

struct MDMTInstance
    M::Int
    L::Int
    l::Int
    dist_matrix::Matrix{Int}
    epsilon::Int
end

# Parses the command line to get its arguments
function parse_commandline()

    settings = ArgParseSettings()
    @add_arg_table settings begin
        "inst"
            help = "a file with an instance of the MDMT problem"
            required = true
        
        "--logfile", "-l"
            help = "a log file to write the output of the program"
            arg_type = String
            default = "log.txt"
        
        "--timelimit", "-t"
            help = "a time limit for the solver, in minutes"
            arg_type = Float64
            default = 180.0
        
    end

    return parse_args(ARGS, settings)

end

# Get a MDMT instance from a file
function parse_instance_file(filename::String)
    data = read(filename, String)
    data = replace(data, r" *\n" => " ")
    data = split(data, " ")
    
    M, L, l = parse.(Int, data[1:3])
    
    dists = data[4:end - 1]
    dists = parse.(Float64, dists)
    dists = reshape(dists, (M, L))

    return MDMTInstance(M, L, l, dists, maximum(dists))
end

function generate_MDMT_model(inst::MDMTInstance)
    m = Model()
    @variable(m, x[1:inst.M], Int)
    @variable(m, s[1:inst.L], Bin)
    @constraint(m, sum(s) == inst.l)
    @constraint(m, [i in 1:inst.M, j in 1:inst.L], x[i] <= inst.dist_matrix[i, j] + inst.epsilon*(1 - s[j]))    
    @objective(m, Max, sum(x))
    return m
end







args = parse_commandline()

@info "[$(Dates.format(now(), RFC1123Format))] Parsing data..."
inst = parse_instance_file(args["inst"])

@info "[$(Dates.format(now(), RFC1123Format))] Instantiating model..."
model = generate_MDMT_model(inst)

@info "[$(Dates.format(now(), RFC1123Format))] Optimizing..."
set_optimizer(model, GLPK.Optimizer)
if haskey(args, "timelimit")
    set_time_limit_sec(model, 60 * args["timelimit"])
    @info "[$(Dates.format(now(), RFC1123Format))] The solver has been set to a time limit of $(time_limit_sec(model) / 60) minutes"
end
optimize!(model)
@info "[$(Dates.format(now(), RFC1123Format))] Done!"

if haskey(args, "logfile")
    open(args["logfile"], "a") do file
        println(file, "[$(Dates.format(now(), RFC1123Format))]")
        println(file, "Instance file: $(args["inst"])")
        println(file, "Solver info:")
        print(file, solution_summary(model))
        println(file, "----------------------------")
    end
else
    print(solution_summary(model))
end

